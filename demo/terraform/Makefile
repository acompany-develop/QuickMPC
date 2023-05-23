create-gcp-instance:
	cd gcp/ && \
	terraform init && \
	terraform plan && \
	terraform apply && \
	terraform output -json > ../application/output.json

deploy-quickmpc:
	cd application && \
	terraform init && \
	terraform plan && \
	terraform apply

delete-gcp-instance:
	cd application/ && \
	terraform destroy && \
	cd ../gcp/ && \
	terraform destroy -auto-approve