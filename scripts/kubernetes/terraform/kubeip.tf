# ---------------------------
# kubeip
# ---------------------------
resource "null_resource" "get-kubeip" {
    provisioner "local-exec" {
        command = "if [ ! -d ./kubeip ]; then git clone https://github.com/doitintl/kubeip; fi"
        working_dir = "../src"
    }
}

resource "null_resource" "kubeip" {
    provisioner "local-exec" {
        command = "chmod +x kubeip.sh && ./kubeip.sh"
        working_dir = "../src"
        environment = {
            GCP_REGION           = var.region
            GCP_ZONE             = "${var.region}-${var.zone}"
            GKE_CLUSTER_NAME     = "${var.instance_name}-gke"
            PROJECT_ID           = var.project_id
            KUBEIP_NODEPOOL      = "default-pool"
            KUBEIP_SELF_NODEPOOL = "default-pool"
            INSTANCE_COUNT       = var.instance_count
            INSTANCE_NAME        = var.instance_name
        }
    }
    depends_on = [null_resource.get-credential, null_resource.get-kubeip]
}
