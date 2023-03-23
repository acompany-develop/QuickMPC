gen_go(){
    find $@ -type f -name *proto | xargs -t -I{} bash generate_go.sh {}
}
gen_python(){
    cd ../packages/client/libclient-py/
    pip install .
    cd quickmpc/proto
    python generate_grpc.py
}

go_dir=("./*manage*/" "./*bts*/" "./common_types/")
python_dir=("./*libc*/" "./common_types/")

if [ $# -eq 0 ]; then
    gen_go ${go_dir[@]}
    gen_python
else
    if [ "`echo ${go_dir[@]} | grep $1`" ]; then
        gen_go $1
    else
        echo "There is no code that needs to be generated"
    fi

    if [ "`echo ${python_dir[@]} | grep $1`" ]; then
        gen_python
    fi
fi
