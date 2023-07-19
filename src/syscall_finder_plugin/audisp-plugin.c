#define _GNU_SOURCE
#include <stdio.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <libaudit.h>
#include <auparse.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <dirent.h>

/* Defining the bits for the two ABIs, 1 and 2 */
#define bit_x86 1         // 0x40000003
#define bit_x86_64 1 << 1 // 0xc000003c

/* This struct is for the callback voidpointer */
typedef struct callback_data {
  bool hasToBeUpdated;
  int calls[512];
  int filterNr;
  char bin_name[256];
} callback_data;

/* This callback function is called whenever an event is emitted */
static void handle_event(auparse_state_t *au, auparse_cb_event_t cb_event_type, void *user_data) {
  if (cb_event_type != AUPARSE_CB_EVENT_READY) return;

  int callNr, num = 0, type, ucheck = 0;
  char arch86[16], arch86_64[16], bin_name[32]; //dir_name[32];
  char *end, *tok, *commcpy;
  const char *arch, *comm, *syscall;
  /* Get the ABI hexcode from the system */
  snprintf(arch86, sizeof(arch86),"%x", AUDIT_ARCH_I386);         // 0x40000003
  snprintf(arch86_64, sizeof(arch86_64),"%x", AUDIT_ARCH_X86_64); // 0xc000003c
  /* Set the pointer to the callback data so the modifications persist through the function scope */
  callback_data *cb_dptr = (callback_data*)user_data;

  while (auparse_goto_record_num(au, num) > 0) {
    type = auparse_get_type(au);
    /* Check if the event is type seccomp */
    if (type == AUDIT_SECCOMP) {
      /* Assign the name (comm), the arch and the syscall number to a variable */
      if ((comm = auparse_find_field(au, "comm")) && (arch = auparse_find_field(au, "arch")) && (syscall = auparse_find_field(au, "syscall"))) {
        /* Change the type so strtok can be used */
        commcpy = strdup(comm);
        /* Remove the quotation marks from the string, since it's literally "file"  */
        tok = strtok(commcpy, "\"");
        /* Check if the name of the binary is the one that's targeted */
        if (strcmp(tok, cb_dptr->bin_name) == 0) {
          /* Preparing for the check if anything has changed, for file creation later on */
          callNr = strtol(syscall, &end, 10);
          ucheck = cb_dptr->calls[callNr];
          /* Depending on the architecture, append either one bit or the other with 'or' */
          if (strcmp(arch86_64, arch) == 0) {
            cb_dptr->calls[callNr] |= bit_x86_64;
          } else if (strcmp(arch86, arch) == 0) {
            cb_dptr->calls[callNr] |= bit_x86;
          }
          /* Set the variable for the change check */
          if (ucheck != cb_dptr->calls[callNr]) cb_dptr->hasToBeUpdated = true;
        }
        free(commcpy);
      }
    }
    num++;
  }
}

int main(int argc, char *argv[]) {
  auparse_state_t *au = NULL;
  char filterName[256], dirName[128], tmp[MAX_AUDIT_MESSAGE_LENGTH];
  FILE* nextFilter;
  struct stat st = {0};
  struct stat st2 = {0};
  struct callback_data cb_data = {0};
  cb_data.hasToBeUpdated = false;
  //cb_data.bin_name = "poc-bin"; 
  const char* dest_dir = "/tmp/syscall-finder";

  char *end;
  struct dirent **filelist;
  int n;

  //https://stackoverflow.com/questions/7430248/creating-a-new-directory-in-c
  struct stat st3 = {0};
  if(stat(dest_dir, &st3)==-1){
      mkdir(dest_dir, 0766);
  }

  n = scandir(dest_dir, &filelist, NULL, alphasort);
  /* Return if there's an error */
  if (n == -1) {
    perror("scandir:");
    return 1;
  }
  while (n--) {
    if (strcmp(filelist[n]->d_name, ".") != 0 && strcmp(filelist[n]->d_name, "..") != 0) {
      //cb_data.bin_name = filelist[n]->d_name;
      snprintf(cb_data.bin_name, sizeof(cb_data.bin_name), "%s", filelist[n]->d_name);
      printf("%s\n", filelist[n]->d_name);
    }
    free(filelist[n]);
  }
  free(filelist);
  /* Create result directory if it doesn't exist yet */
  if(stat(dest_dir, &st) == -1) mkdir(dest_dir, 0775);

  /* Initialize the auparse library */
  au = auparse_init(AUSOURCE_FEED, 0);
  if (au == NULL) {
		return 2;
  }
  /* Add the callback function */
  auparse_add_callback(au, handle_event, &cb_data, NULL);
  do {
    int retval = -1;
    fd_set read_mask;

    FD_ZERO(&read_mask);
    FD_SET(0, &read_mask);

    do {
      retval = select(1, &read_mask, NULL, NULL, NULL);
    } while (retval == -1 && errno == EINTR);
  
    // Now the event loop
    if (retval > 0) {
      if (fgets_unlocked(tmp, MAX_AUDIT_MESSAGE_LENGTH, stdin)) {
        auparse_feed(au, tmp, strnlen(tmp, MAX_AUDIT_MESSAGE_LENGTH));
      }
    } else if (retval == 0) auparse_flush_feed(au);

    /* Create a new filter when the data has changed */
    auparse_flush_feed(au); 
    if (cb_data.hasToBeUpdated == true && cb_data.bin_name != "") {
      cb_data.hasToBeUpdated = false;
      /* Generate the dir name  
      /tmp/syscall-finder/binaryname */
      snprintf(dirName, sizeof(dirName), "%s/%s", dest_dir, cb_data.bin_name);
      /* Create the directory if it doesn't exist */
      if(stat(dirName, &st) == -1) mkdir(dirName, 0755);
      /* Generate new filter name with path
      /tmp/syscall-finder/binaryname/number */
      snprintf(filterName, sizeof(filterName), "%s/%d", dirName, cb_data.filterNr);
      /* Create the file */
      nextFilter = fopen(filterName, "w+");
      if (nextFilter == NULL) {
        perror("Could not open file");
        fprintf(stderr, "Filename was %s\n", filterName);
        return 1;
      }
      for (int i = 0; i < 512; i++) {
        fprintf(nextFilter, "%d", cb_data.calls[i]);
      }
      fclose(nextFilter);
      /* Increase the filter number for the next iteration */
      cb_data.filterNr++;
    }
    if (feof(stdin)) break;
  } while (1);

  /* Flush any accumulated events from queue */
  auparse_flush_feed(au);
  auparse_destroy(au);

  /* Create final filter after flushing
  This is for testing purposes, the loop only exits when audispd stops the plugin */
  snprintf(dirName, sizeof(dirName), "%s/%s", dest_dir, cb_data.bin_name);
  snprintf(filterName, sizeof(filterName), "%s/%d", dirName, cb_data.filterNr);
  printf("%s\n", filterName);
  nextFilter = fopen(filterName, "w+");
  if (nextFilter == 0) {
    perror("Could not open file");
    fprintf(stderr, "Filename was %s\n", filterName);
    return 1;
  }
  for (int i = 0; i < 512; i++) {
    fprintf(nextFilter, "%d", cb_data.calls[i]);
  }
  fclose(nextFilter);

  return 0;
}