var active_connection = null;

function addMessageLog(name, message) {
  document.getElementById("message_log").innerHTML += "<b>" + name + "</b>: " + message + "<br>";
}

function onConnected(connection) {
  active_connection = connection;
  document.getElementById("connection_status").innerText = "Connected with " + active_connection.peer;
  active_connection.on("data", function (data) {
    addMessageLog(data.name, data.message);
  });
  active_connection.on("close", function () {
    document.getElementById("video").src = "";
    document.getElementById("connection_status").innerText = "Not connected";
    document.getElementById("connect_div").style.display = "block";
    document.getElementById("disconnect_div").style.display = "none";
  });
  document.getElementById("connect_div").style.display = "none";
  document.getElementById("disconnect_div").style.display = "block";
}

window.onload = function () {
  var peer = new Peer({key: 'its5por6n2i96bt9'});
  peer.on("open", function (id) {
    document.getElementById("id").innerText = id;
  });

  document.getElementById("connection_status").innerText = "Not connected";
  peer.on("connection", function (connection) {
    onConnected(connection);
  });
  peer.on("call", function (call) {
    navigator.webkitGetUserMedia({video: true, audio: false}, function (stream) {
      call.answer(stream);
      call.on("stream", function (remoteStream) {
        document.getElementById("video").src = URL.createObjectURL(remoteStream);
      });
    }, function (error) {
      document.getElementById("connection_status").innerText = "Stream error with " + call_id + ": " + error;
    });
  });

  document.getElementById("call_button").addEventListener("click", function (event) {
    var name = document.getElementById("name").value || peer.id;
    document.getElementById("connection_status").innerText = "Connecting...";
    var call_id = document.getElementById("call_id").value;
    connection = peer.connect(call_id);
    connection.on("open", function () {
      onConnected(connection);
      active_connection.send({name: name, message: "< Joined the chat >"});
      addMessageLog(name, "< Joined the chat >");
    });

    navigator.webkitGetUserMedia({video: true, audio: false}, function (stream) {
      var call = peer.call(call_id, stream);
      call.on("stream", function (remoteStream) {
        document.getElementById("video").src = URL.createObjectURL(remoteStream);
      });
    }, function (error) {
      document.getElementById("connection_status").innerText = "Stream error with " + call_id + ": " + error;
    });
  });

  document.getElementById("disconnect_button").addEventListener("click", function (event) {
    var name = document.getElementById("name").value || peer.id;
    if (active_connection) {
      active_connection.send({name: name, message: "< Left the chat >"});
      setTimeout(function () {
        active_connection.close();
        addMessageLog(name, "< Left the chat >");
      }, 1000);
    }
    document.getElementById("connection_status").innerText = "Not connected";
  });

  document.getElementById("send_button").addEventListener("click", function (event) {
    var name = document.getElementById("name").value || peer.id;
    var message = document.getElementById("send_message").value;
    document.getElementById("send_message").value = "";
    if (active_connection) {
      active_connection.send({name: name, message: message});
    }
    addMessageLog(name, message);
  });
}
