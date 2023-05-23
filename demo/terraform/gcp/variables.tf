# ---------------------------
# Environment variables
# ---------------------------
variable "instance_count" {
  # NOTE: この数値の数パーティ分だけ環境が立ち上がる(btsを含めたパーティ数を記載する)
  default = 1
}

variable "service_account_json" {
  # NOTE: README.mdを参考に参考に取得したjsonをterraform/gcp/jsonに配置してその絶対パスを設定する
  default = "/path/to/json"
}

variable "instance_name" {
  # NOTE: リソース名のprefixとなるので適切な名前を設定する
  default = "terraform-qmpc-k8s"
}

variable "project_id" {
  # NOTE: リソースを立ち上げたいgcp_projectのIDを設定する。projectを開いてurlを見ると末尾の方に書いてある
  default = "hoge-huga-123456"
}

variable "zone" {
  # NOTE: リソースを立ち上げたいリージョンを設定する
  default = "asia-east2-a"
}

variable "mechine_type" {
  # NOTE: インスタンスタイプを設定する
  default = "e2-medium"
}

variable "disk_image" {
  # NOTE: osのイメージを設定する。`gcloud compute images list`で全ての設定可能な値が確認できる
  default = "ubuntu-2004-focal-v20221018"
}

variable "disk_type" {
  # NOTE: https://cloud.google.com/compute/docs/disks?hl=ja#disk-types を参照
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
