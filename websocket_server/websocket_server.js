const fs = require("fs");
const https = require("https");
const WebSocket = require("ws");

const server = https.createServer({
  cert: fs.readFileSync("/etc/letsencrypt/live/haraken.info/cert.pem"),
  key: fs.readFileSync("/etc/letsencrypt/live/haraken.info/privkey.pem")
});

const wss = new WebSocket.Server({ server });
let counter = 0;

wss.on("connection", ws => {
  // console.log(ws);
  ws.on("message", json => {
    try {
      const packet = JSON.parse(json);
      // console.log(packet);
      if (packet.type == "CONNECT") {
        if (packet.meet_id) {
          ws.meetID = packet.meet_id;
        }
      } else if (packet.type == "STAMP") {
        if (packet.meet_id) {
          // console.log("STAMP");
          // console.log(ws.meetID);
          counter++;
          if (counter % 100 == 0) {
            console.log(counter);
          }
          ws.meetID = packet.meet_id;
          wss.clients.forEach(client => {
            if (client.meetID == packet.meet_id) {
              client.send(json);
            }
          });
        }
      }
    } catch (e) {
      console.log(e);
      ws.close();
    }
  });
});

server.listen(5001);
