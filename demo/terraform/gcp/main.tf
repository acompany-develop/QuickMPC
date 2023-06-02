# ---------------------------
# Setup provider
# ---------------------------
provider "google" {
  credentials = file("${var.service_account_json}")
  project     = var.project_id
  zone        = "${var.region}-${var.zone}"
}

# get my public-ip
provider "http" {}
