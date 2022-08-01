// azureサーバの1台目が使用
// main.jsはslackとやり取りをするが、sub.jsはしない
const express = require("express");
const axios = require('axios');
const fs = require('fs');
const https = require('https');
const util = require('util');
const childProcess = require('child_process');
const exec = util.promisify(childProcess.exec);
const schedule = require('node-schedule');
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
const ENDPOINT2 = "https://ec2-3-20-7-71.us-east-2.compute.amazonaws.com:3001";
const ENDPOINT3 = "https://ec2-3-22-90-141.us-east-2.compute.amazonaws.com:3001";

if (!AUTH_TOKEN) {
    console.log('Error: Specify CI_AUTH_TOKEN in environment');
    process.exit(1);
}

let jobs = []; // 待機中のジョブを管理
let isJobRunning = false; // ジョブが実行中かどうか

let jobs_status = new Map();

// ジョブを実行する
async function executeJob() {
    if (jobs.length == 0 || isJobRunning) return;

    const job = jobs.shift();
    console.log("job:", job);
    console.log("joblist:", jobs);
    isJobRunning = true; // ジョブ実行中のフラグ立てる
    jobs_status.set(job.id, "doing");

    const httpsAgent = new https.Agent({ rejectUnauthorized: false });

    // マシン2に起動リクエストを送信
    axios({
        method: 'post',
        url: ENDPOINT2 + '/up',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': 'Bearer ' + AUTH_TOKEN
        },
        data: {
            job_id: job.id
        },
        httpsAgent: httpsAgent,
        timeout: 60000
    })
    .catch((err) => {
        console.log(err);
    })
    // マシン3に起動リクエストを送信
    axios({
        method: 'post',
        url: ENDPOINT3 + '/up',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': 'Bearer ' + AUTH_TOKEN
        },
        data: {
            job_id: job.id
        },
        httpsAgent: httpsAgent,
        timeout: 60000
    })
    .catch((err) => {
        console.log(err);
    })

    // コンテナ起動スクリプトを実行
    const up = exec(`./up.sh ${job.id}`).catch((err) => {
        isJobRunning = false;
        jobs_status.set(job.id, "failed");
        console.log(err);
    });

    // コンテナ監視スクリプトを実行
    const watch = await exec(`./watch.sh ${job.number} "${job.title}" ${job.url} ${job.id} ${AUTH_TOKEN}`).catch((err) => {
        isJobRunning = false;
        jobs_status.set(job.id, "failed");
        console.log(err);
    });
    console.log(watch.stdout);
    isJobRunning = false;
}

// 毎分実行する
schedule.scheduleJob( '* * * * *', function(){
    executeJob();
});

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

// 認証必須API
// jobのステータスをチェック
app.get('/job', auth, (req, res) => {
    const job_id = req.query.job_id;
    const status = jobs_status.get(job_id);

    if(status == "passed" || status == "failed") {
        jobs_status.delete(job_id);  // waitingとdoing以外ならjobが完了しているので、mapから削除する
    }

    res.status(200).end(status); // status を返す
});

// 認証必須API
// jobのステータスを更新
app.put('/job', auth, (req, res) => {
    const job_id = req.body.job_id;
    const status = req.body.status;
    jobs_status.set(job_id, status);
    console.log("put", jobs_status);

    res.status(200).end(`OK!`);
});

// 認証必須API
app.post('/ci', auth, (req, res) => {
    console.log("request reveived!")

    const number = encodeURI(req.body.pr_number);
    const title = encodeURI(req.body.pr_title);
    const url = encodeURI(req.body.pr_url);
    const job_id = req.body.run_id;

    // jobを追加
    jobs.push({ id: job_id, number: number, title: title, url: url });

    jobs_status.set(job_id, "waiting");
    console.log("post", jobs_status);

    res.status(200).end(job_id);
});

// エラーハンドリング
app.use((err, req, res, next)=>{
    res.status(500).send(err)
});

server.listen(PORT, () => console.info('listen: ', PORT));