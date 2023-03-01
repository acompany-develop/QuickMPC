# ---------------------------
# Environment variables
# ---------------------------
variable "instance_count" {
  # この数値の数パーティ分だけ環境が立ち上がる
  default = 1
}

variable "service_account_json" {
  default = "/root/QuickMPC/scripts/terraform/gcp/json/noted-gizmo-337508-7d9dfbf4fe18.json"
}

variable "instance_name" {
  default = "terraform-qmpc-k8s"
}

variable "project_id" {
  default = "noted-gizmo-337508"
}

variable "zone" {
  default = "asia-east2-a"
}

variable "mechine_type" {
  default = "e2-medium"
}

variable "disk_image" {
  default = "ubuntu-2004-focal-v20221018"
}

variable "disk_type" {
  default = "pd-ssd"
}

variable "subnet_cidr_range" {
  default = "10.0.1.0/24"
}

variable "key_name" {
  default = "qmpc-k8s"
}

variable "gce_ssh_user" {
  default = "ubuntu"
}
