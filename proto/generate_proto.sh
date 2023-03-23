gen_go(){
    find $1 -type f -name *proto | xargs -t -I{} bash generate_go.sh {}
}
gen_python(){
    cd ../packages/client/libclient-py/quickmpc/proto
    python generate_grpc.py
}

if [ $# -eq 0 ]; then
    gen_go "./*manage*"
    gen_go "./*bts*"
    gen_go "./common_types"
    gen_python
else
    gen_go $1
    if [[ "$1" == *"libc"* || "$1" == *"common_types"* ]]; then
        gen_python
    fi
fi
