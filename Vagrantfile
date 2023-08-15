# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  config.vm.define "ubuntu_jammy_desktop"
  config.vm.box = "ubuntu/jammy64"
  config.vagrant.plugins = "vagrant-vbguest"
  config.vm.provider "virtualbox" do |vb|
    vb.cpus = 2
    vb.gui = true
    vb.memory = "8192"
    vb.customize ["modifyvm", :id, "--accelerate3d", "on"]
    vb.customize ['modifyvm', :id, '--graphicscontroller', 'vmsvga']
    vb.customize ["modifyvm", :id, "--vram", "128"]
    vb.customize ['modifyvm', :id, '--clipboard-mode', 'bidirectional']
    vb.customize ['modifyvm', :id, '--draganddrop', 'bidirectional']
   # vb.customize ["modifyvm", :id, "--ioapic", "on"]
    vb.customize ["modifyvm", :id, "--usb", "on"]
    vb.customize ["modifyvm", :id, "--mouse", "usbtablet"]
  end
  config.vm.synced_folder "vagrant" , "/vagrant", :mount_options => ["ro"]
  config.vm.synced_folder "synced_folder" , "/synced_folder/", type: "virtualbox", :mount_options => ['dmode=777','fmode=775'], automount: true
  config.vm.provision "file", source: "src", destination: "/home/vagrant/fuzzypol/"
  config.vm.provision "file", source: "afl_custom_mutators", destination: "/home/vagrant/fuzzypol/"



  if Vagrant.has_plugin?("vagrant-vbguest") then
    config.vbguest.installer_options = { running_kernel_modules: ["vboxguest"] }
    config.vbguest.auto_update = false
  end
  # Install xfce and virtualbox additions
  config.vm.provision "shell", inline: "sudo apt-get update"
  config.vm.provision "shell", inline: "sudo apt-get install -y ubuntu-desktop build-essential dkms virtualbox-guest-utils virtualbox-guest-x11"
  # Permit anyone to start the GUI
  config.vm.provision "shell", inline: "sudo sed -i 's/allowed_users=.*$/allowed_users=anybody/' /etc/X11/Xwrapper.config"
  # Permit password authentication because broken vagrant publickey authentication
  config.vm.provision "shell", inline: "sudo sed -i 's/PasswordAuthentication no/PasswordAuthentication yes/g' /etc/ssh/sshd_config; sudo systemctl restart sshd;", run: "always"
  
  
  config.vm.provision "ansible_local" do |ansible|
    ansible.verbose = "v" 
    ansible.playbook = "master_playbook.yml"
    ansible.provisioning_path = "/vagrant/provisioning"
    ansible.inventory_path = "inventory"
    ansible.limit = "all"
  end

  config.vm.provision "install_klee", type: "ansible_local" do |ansible|
    ansible.verbose = "v" 
    ansible.playbook = "klee_installation.yml"
    ansible.provisioning_path = "/vagrant/provisioning"
    ansible.inventory_path = "inventory"
    ansible.limit = "all"
  end

  config.vm.provision "install_examples", type: "ansible_local" do |ansible|
    ansible.verbose = "v" 
    ansible.playbook = "install_examples.yml"
    ansible.provisioning_path = "/vagrant/provisioning"
    ansible.inventory_path = "inventory"
    ansible.limit = "all"
  end
  
  config.vm.provision "Takes long: install_sysfilter_tempSpecial_chestnut", type: "ansible_local" do |ansible|
    ansible.verbose = "v" 
    ansible.playbook = "install_sysfilter_tempSpecial_chestnut.yml"
    ansible.provisioning_path = "/vagrant/provisioning"
    ansible.inventory_path = "inventory"
    ansible.limit = "all"
  end
  
  config.vm.provision "Install Testbench", type: "ansible_local" do |ansible|
    ansible.verbose = "v" 
    ansible.playbook = "compile_3_examples.yml"
    ansible.provisioning_path = "/vagrant/provisioning"
    ansible.inventory_path = "inventory"
    ansible.limit = "all"
  end
  
  config.vm.provision "Analyze Testbench", type: "ansible_local" do |ansible|
    ansible.verbose = "v" 
    ansible.playbook = "analyze_3_examples.yml"
    ansible.provisioning_path = "/vagrant/provisioning"
    ansible.inventory_path = "inventory"
    ansible.limit = "all"
  end
end
