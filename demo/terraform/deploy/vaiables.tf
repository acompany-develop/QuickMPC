locals {
  json_data = jsondecode(file("./output.json"))
  instance_count = local.json_data["instance_count"]["value"]
  qmpc_instance_gips = local.json_data["qmpc_instance_gips"]["value"]
  private_key_path = local.json_data["private_key_path"]["value"]
  gce_ssh_user =  local.json_data["gce_ssh_user"]["value"]
  docker_image_tag = "0.3.8"
}

variable "docker_image_tag" {
  default = "0.3.8"
}
