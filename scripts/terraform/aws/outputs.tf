output "qmpc_instance_gip" {
  value = aws_instance.qmpc_k8s_ec2.public_ip
}
