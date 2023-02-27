# ---------------------------
# Key pair
# ---------------------------
locals {
  public_key_file = "./.key_pair/${var.key_name}.id_rsa.pub"
  private_key_file = "./.key_pair/${var.key_name}.id_rsa"
}

resource "tls_private_key" "qmpc_k8s_keygen" {
  algorithm = "RSA"
  rsa_bits = 4096
}

resource "local_file" "private_key_pem" {
  filename = local.private_key_file
  content = tls_private_key.qmpc_k8s_keygen.private_key_pem
  file_permission = "0600"
}

resource "local_file" "public_key_pem" {
  filename = local.public_key_file
  content = tls_private_key.qmpc_k8s_keygen.public_key_openssh
  file_permission = "0600"
}

resource "aws_key_pair" "qmpc_k8s_key_pair" {
  key_name = var.key_name
  public_key = tls_private_key.qmpc_k8s_keygen.public_key_openssh
}

# ---------------------------
# Ec2
# ---------------------------
resource "aws_instance" "qmpc_k8s_ec2" {
  ami                         = var.ami
  instance_type               = var.instance_type
  availability_zone           = var.az_a
  vpc_security_group_ids      = [aws_security_group.qmpc_k8s_sg.id]
  subnet_id                   = aws_subnet.qmpc_k8s_sn.id
  associate_public_ip_address = "true"
  key_name                    = aws_key_pair.qmpc_k8s_key_pair.id

  tags = {
    Name = "terraform-qmpc-k8s-ec2"
  }

  root_block_device {
    volume_size = var.volume_size
  }
}
