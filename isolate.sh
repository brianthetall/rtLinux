sudo systemctl set-property --runtime custom-workload.slice AllowedCPUs=0
sudo systemctl set-property --runtime init.scope AllowedCPUs=1-3
sudo systemctl set-property --runtime system.slice AllowedCPUs=1-3
sudo systemctl set-property --runtime user.slice AllowedCPUs=1-3

#systemd-run --scope -p Slice=custom-workload.slice /home/ubuntu/my-ap
