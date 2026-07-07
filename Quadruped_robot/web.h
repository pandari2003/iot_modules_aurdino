#ifndef WEB_H
#define WEB_H

const char webpage[] PROGMEM = R"rawliteral(

<!DOCTYPE html>
<html>

<head>

<meta name="viewport" content="width=device-width, initial-scale=1">

<style>

body{

font-family:Arial;
background:#222;
color:white;
text-align:center;

}

button{

width:150px;
height:45px;
margin:10px;
font-size:18px;

}

input{

width:250px;

}

</style>

</head>

<body>

<h2>Quadruped</h2>

<button onclick="send('center')">CENTER</button>
<button onclick="send('stand')">STAND</button>
<button onclick="send('sit')">SIT</button>

<hr>

<button onclick="send('forward')">FORWARD</button>

<br><br>

<button onclick="send('left')">LEFT</button>

<button onclick="send('stop')">STOP</button>

<button onclick="send('right')">RIGHT</button>

<br><br>

<button onclick="send('backward')">BACKWARD</button>

<hr>

Speed

<input type="range" id="speed" min="5" max="60" value="20">

<span id="sp">15</span>

<button onclick="setSpeed()">SET SPEED</button>
 
<script>

let speed = document.getElementById("speed");

speed.oninput = function()
{
    document.getElementById("sp").innerHTML = this.value;
}

function send(cmd)
{
    fetch("/" + cmd);
}

function setSpeed()
{
    fetch("/speed?value=" + speed.value);
}

let speed = document.getElementById("speed");

speed.oninput = function()
{
    document.getElementById("sp").innerHTML = this.value;
}

function send(cmd)
{
    fetch("/" + cmd)
    .then(response => response.text())
    .then(data => console.log(data));
}

function setSpeed()
{
    fetch("/speed?value=" + speed.value)
    .then(response => response.text())
    .then(data => console.log(data));
}

</script>

</body>

</html>

)rawliteral";

#endif