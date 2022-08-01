// azureサーバの2, 3台目が使用
// main.jsはslackとやり取りをするが、sub.jsはしない
const express = require("express");
const fs = require('fs');
const https = require('https');
const util = require('util');
const childProcess = require('child_process');
const exec = util.promisify(childProcess.exec);
require('dotenv').config();

const PORT = 3001;
const options = {
    key:  fs.readFileSync('./Server.key'),
    cert: fs.readFileSync('./Server.crt')
};

const app = express();
app.use(express.urlencoded({
    extended: true
}));
app.use(express.json());
const server = https.createServer(options, app);

app.use(express.json())
app.use(express.urlencoded({ extended: true }));

const AUTH_TOKEN = process.env.CI_AUTH_TOKEN;

if (!AUTH_TOKEN) {
    console.log('Error: Specify CI_AUTH_TOKEN in environment');
    process.exit(1);
}

// 認証用ミドルウェア
const auth = (req, res, next) => {
    // リクエストヘッダーからトークンの取得
    let token = '';
    if (req.headers.authorization &&
        req.headers.authorization.split(' ')[0] === 'Bearer') {
        token = req.headers.authorization.split(' ')[1];
    } else {
        return next('token none');
    }

    // トークンの検証
    if (token === AUTH_TOKEN) {
        next();
    } else {
        next('invalid token');
    }
}

app.post('/up', auth, (req, res) => {
    console.log("request reveived!")
    res.status(200).send(`OK!`);

    const job_id = req.body.job_id;
    // コンテナ起動スクリプトを実行
    exec(`./up.sh ${job_id}`).catch((err) => {
        console.log(err);
    });
});

// エラーハンドリング
app.use((err, req, res, next)=>{
    res.status(500).send(err)
});

server.listen(PORT, () => console.info('listen: ', PORT));