create-gcp-instance:
	cd gcp/ && \
	terraform init && \
	terraform plan && \
	terraform apply && \
	terraform output -json > ../deploy/output.json

deploy-quickmpc:
	cd deploy && \
	terraform init && \
	terraform plan && \
	terraform apply

delete-gcp-instance:
	cd deploy/ && \
	terraform destroy && \
	cd ../gcp/ && \
	terraform destroy -auto-approve