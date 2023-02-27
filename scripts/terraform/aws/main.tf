# ---------------------------
# Setup provider
# ---------------------------
provider "aws" {
  region = var.region
}

# get my public-ip
provider "http" {}
