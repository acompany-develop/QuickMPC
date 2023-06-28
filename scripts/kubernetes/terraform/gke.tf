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


# ---------------------------
# static ip address
# ---------------------------
resource "google_compute_address" "qmpc_k8s_static_ip" {
  count      = var.instance_count
  name       = "${var.instance_name}-ip${count.index}"
  project    = var.project_id
  region     = var.region
}

# ---------------------------
# K8s cluster
# ---------------------------
resource "google_container_cluster" "gke" {
  count              = var.instance_count
  name               = "${var.instance_name}-gke${count.index}"
  location           = "${var.region}-${var.zone}"
  initial_node_count = 1

  release_channel {
    # NOTE: clusterの更新頻度
    # UNSPECIFIED: Not set.
    # RAPID: Weekly upgrade cadence; Early testers and developers who requires new features.
    # REGULAR: Multiple per month upgrade cadence; Production users who need features not yet offered in the Stable channel.
    # STABLE: Every few months upgrade cadence; Production users who need stability above all else, and for whom frequent upgrades are too risky.
    channel = "UNSPECIFIED"
  }

  networking_mode = "VPC_NATIVE"
  ip_allocation_policy {
    cluster_ipv4_cidr_block  = "10.10.0.0/16"
    services_ipv4_cidr_block = "10.20.0.0/16"
  }

  network    = google_compute_network.qmpc_k8s_vpc.*.self_link[count.index]
  subnetwork = google_compute_subnetwork.qmpc_k8s_sn.*.self_link[count.index]

  addons_config {
    http_load_balancing {
      disabled = true
    }
    horizontal_pod_autoscaling {
      disabled = true
    }
  }

  node_config {
    machine_type = var.mechine_type
    disk_type    = var.disk_type
    disk_size_gb = var.disk_size
  }
}

# ---------------------------
# get credential
# ---------------------------
resource "null_resource" "get-credential" {
  triggers = {
    instance_name = var.instance_name
    region = var.region
    zone = var.zone
    project_id = var.project_id
    instance_count = var.instance_count
  }
  provisioner "local-exec" {
      command = "for ((i=0; i<$INSTANCE_COUNT; i++)); do gcloud container clusters get-credentials $GKE_CLUSTER_NAME$i --region $GCP_ZONE --project $PROJECT_ID; done"
      environment = {
        GCP_ZONE             = "${var.region}-${var.zone}"
        GKE_CLUSTER_NAME     = "${var.instance_name}-gke"
        PROJECT_ID           = var.project_id
        INSTANCE_COUNT       = var.instance_count
      }
  }

  # NOTE: destroy時にcontextを削除する
  provisioner "local-exec" {
      command = "for ((i=0; i<${self.triggers.instance_count}; i++)); do kubectl config delete-context gke_${self.triggers.project_id}_${self.triggers.region}-${self.triggers.zone}_${self.triggers.instance_name}-gke$i; done"
      when = destroy
  }
  depends_on = [google_container_cluster.gke, null_resource.get-kubeip]
}
