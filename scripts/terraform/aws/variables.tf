# ---------------------------
# Environment variables
# ---------------------------
variable "region" {
  default = "ap-southeast-1"
}

variable "az_a" {
  default = "ap-southeast-1a"
}

variable "ami" {
  default = "ami-04ff9e9b51c1f62ca"
}

variable "instance_type" {
  default = "c5.large"
}

variable "volume_size" {
  default = "128"
}

variable "key_name" {
  default = "qmpc-k8s"
}

variable "vpc_cidr_block" {
  default = "10.0.0.0/16"
}

variable "subnet_cidr_block" {
  default = "10.0.1.0/24"
}
