create-gcp-instance:
	cd gcp/ && \
	terraform init && \
	terraform plan && \
	terraform apply && \
	terraform output -json > ../deploy/output.json

delete-gcp-instance:
	cd gcp/ && \
	terraform destroy