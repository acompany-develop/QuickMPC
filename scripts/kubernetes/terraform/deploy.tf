# ---------------------------
# create manifest
# ---------------------------

resource "null_resource" "create_manifest" {
    provisioner "local-exec" {
        command = "chmod +x manifest.sh && ./manifest.sh"
        working_dir = "../src"
        environment = {
           PARTY_LIST = join(",", google_compute_address.qmpc_k8s_static_ip.*.address)
        }
    }
}

# ---------------------------
# create manifest
# ---------------------------

resource "null_resource" "deploy" {
    provisioner "local-exec" {
        command = "chmod +x apply.sh && ./apply.sh"
        working_dir = "../src"
        environment = {
            GCP_ZONE             = "${var.region}-${var.zone}"
            GKE_CLUSTER_NAME     = "${var.instance_name}-gke"
            PROJECT_ID           = var.project_id
        }
    }
    depends_on = [null_resource.create_manifest,null_resource.kubeip]
}
