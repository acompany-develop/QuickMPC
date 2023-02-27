# ---------------------------
# Vpc
# ---------------------------
resource "aws_vpc" "qmpc_k8s_vpc" {
  cidr_block           = var.vpc_cidr_block
  instance_tenancy     = "default"
  enable_dns_hostnames = true
  enable_dns_support   = true
  tags = {
    Name = "terraform-qmpc-k8s-vpc"
  }
}

# ---------------------------
# Subnet
# ---------------------------
resource "aws_subnet" "qmpc_k8s_sn" {
  vpc_id                  = aws_vpc.qmpc_k8s_vpc.id
  cidr_block              = var.subnet_cidr_block
  availability_zone       = var.az_a
  map_public_ip_on_launch = true
  tags = {
    Name = "terraform-qmpc-k8s-sn"
  }
}

# ---------------------------
# Internet gateway
# ---------------------------
resource "aws_internet_gateway" "qmpc_k8s_igw" {
  vpc_id = aws_vpc.qmpc_k8s_vpc.id

  tags = {
    Name = "terraform-qmpc-k8s-igw"
  }
}

# ---------------------------
# Route table
# ---------------------------
resource "aws_route_table" "qmpc_k8s_rt" {
  vpc_id = aws_vpc.qmpc_k8s_vpc.id
  route {
    cidr_block = "0.0.0.0/0"
    gateway_id = aws_internet_gateway.qmpc_k8s_igw.id
  }

  tags = {
    Name = "terraform-qmpc-k8s-rt"
  }
}

resource "aws_route_table_association" "qmpc_k8s_rt_associate" {
  subnet_id      = aws_subnet.qmpc_k8s_sn.id
  route_table_id = aws_route_table.qmpc_k8s_rt.id
}

# ---------------------------
# Security group
# ---------------------------
# get my public ip
data "http" "ifconfig" {
  url = "http://ipv4.icanhazip.com/"
}

locals {
  myip = chomp(data.http.ifconfig.body)
}

resource "aws_security_group" "qmpc_k8s_sg" {
  name        = "terraform-qmpc-k8s-sg"
  vpc_id      = aws_vpc.qmpc_k8s_vpc.id
  tags = {
    Name = "terraform-qmpc-k8s-sg"
  }

  # inbound rule
  ingress {
    from_port   = 22
    to_port     = 22
    protocol    = "tcp"
    cidr_blocks = ["${local.myip}/32"]
  }

  # outbound rule
  egress {
    from_port   = 0
    to_port     = 0
    protocol    = "-1"
    cidr_blocks = ["0.0.0.0/0"]
  }
}

# # aws_security_groupの定義
# resource "aws_security_group" "qmpc_allow_tcp_from_parties_sg" {
#   name_prefix = "qmpc-allow-tcp-from-parties-sg"
#   vpc_id      = aws_vpc.qmpc_k8s_vpc.id
#   tags = {
#     Name = "terraform-qmpc-k8s-sg"
#   }

#   ingress {
#     from_port = 0
#     to_port   = 65535
#     protocol  = "tcp"
#     cidr_blocks = ["0.0.0.0/0"]
#   }
# }

# # aws_security_groupのingressルールの定義
# resource "aws_security_group_rule" "qmpc_allow_tcp_from_parties_sg_ingress_all_tcp" {
#   security_group_id = "${aws_security_group.qmpc_allow_tcp_from_parties_sg.id}"
#   type              = "ingress"
#   from_port         = 0
#   to_port           = 65535
#   protocol          = "tcp"
#   cidr_blocks       = ["0.0.0.0/0"]
#   description       = "Allow all TCP traffic from elastic IP of the instance"

#   source_security_group_id = "${aws_instance.qmpc_k8s_ec2.elastic_ip}"
# }
