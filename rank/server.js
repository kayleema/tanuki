if (process.env.NODE_ENV !== 'production') {
    require('dotenv').config();
}

// Constants
const PORT = 3000;
const HOST = '0.0.0.0';
const CLIENT_ID = "581212950796-01c78s70trahbfpfq9ds7qahs7gn7aqo.apps.googleusercontent.com"
const MONGO_PASS = process.env.MONGO_PASS
const MONGO_URI = `mongodb+srv://tanuki:${MONGO_PASS}@tanuki-krzkv.gcp.mongodb.net/test?retryWrites=true&w=majority`;

// Imports
const express = require('express');
var cors = require('cors');
const {OAuth2Client} = require('google-auth-library');

const oauthClient = new OAuth2Client(CLIENT_ID);

async function verify(token) {
    const ticket = await oauthClient.verifyIdToken({
        idToken: token,
        audience: CLIENT_ID,
    });
    const payload = ticket.getPayload();
    const userid = payload['sub'];
    return payload;
}


//MongoDB
const MongoClient = require('mongodb').MongoClient


let db;
const client = new MongoClient(MONGO_URI, { useNewUrlParser: true });
client.connect(err => {
    db = client.db("tanuki");
    const collection = db.collection("requests");
    // collection.remove({name: "Kaylee"});
    // perform actions on the collection object
  
    app.listen(PORT, HOST);
    console.log(`Running on http://${HOST}:${PORT}`);
    // client.close();
});

// App
const app = express();
app.use(cors());

app.get('/', (req, res) => {
    res.send('狸の世界は素晴らしいです。');
});

app.get('/profile', async (req, res) => {
    let auth;
    try {
        auth = await verify(req.get("Authorization").split(" ")[1]);
    } catch (error) {
        res.sendStatus(401);
        console.log("bad auth error");
        return;
    }

    const name = auth.given_name;
    const email = auth.email;
    const userId = auth.sub;

    await db.collection('requests').updateOne({userId, userId}, {
        $set: {
            name: name,
            userId: userId,
            email: email,
        },
        $currentDate: {
            lastRequest: true,
        }
    }, {upsert: true});
    const result = await db.collection('requests').findOne({userId, userId});
    
    res.send(result);
})

app.post('/profile/complete/:testId', async (req, res) => {
    let auth;
    try {
        auth = await verify(req.get("Authorization").split(" ")[1]);
    } catch (error) {
        res.sendStatus(401);
        console.log("bad auth error");
        return;
    }

    const name = auth.given_name;
    const userId = auth.sub;
    const testId = req.params.testId;

    await db.collection('requests').updateOne({userId, userId}, {
        $set: {
            [`completed.${testId}`]: true,
        },
    });

    res.send(`いらっしゃいませ、${name}様`);
})
