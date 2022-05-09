"use strict";

function setAttributes(el, attrs) {
	for (var key in attrs) {
		el.setAttribute(key, attrs[key]);
	}
}

const WebSocketUrl = window.location.host + "/ws";
const ProtocolVersion = 1;

var ws;
function webSocketStart() {
	try {
		ws = new WebSocket("ws://" + WebSocketUrl);
	} catch (exception) {
		console.error(exception);
	}
	if (!ws) return;

	ws.onclose = function (evt) {
		console.log("close:\n" + evt.data);
	};
	ws.onerror = function (evt) {
		console.log("error:\n" + evt.data);
	};
	ws.onmessage = function (evt) {
		console.log("message:\n");
		try {
			var jsonObj = JSON.parse(evt.data);
			parseJsonMsg(jsonObj);
		}
		catch (e) {
			console.error("Parsing error:", e);
		}
	};
	ws.onopen = function (evt) {
		console.log("open:\n" + evt.data);
		if (ws) ws.send(genMessage("Init"));
	};
}

function parseJsonMsg(data) {
	if (!data.name || data.name != "DiceWars"
		|| !data.version || data.version != ProtocolVersion
		|| !data.data)
		return;

	parseData(data.data);
}

var currentMap = null;

function parseData(data) {
	if (!Array.isArray(data)) data = [data];

	for (var msg in data) {
		for (var d in data[msg]) {
			switch (d) {
			case "map":
				currentMap = data[msg][d];
				initMap();
				break;

			case "players":
				if (Array.isArray(data[msg][d])) {
					for (var i in data[msg][d]) {
						if (data[msg][d][i].player) {
							initPlayer(data[msg][d][i].player);
						}
					}
				}
				break;

			case "player":
				initPlayer(data[msg][d]);
				break;

			case "state":
				setState(data[msg][d]);
				break;
			}
		}
	}
	endInit();
}

function genMessage(val) {
	return (JSON.stringify({ "name": "DiceWars", "version": ProtocolVersion, "request": val }));
}

function webSocketClose() {
	if (ws) {
		ws.close();
		ws = null;
	}
}

var isLoaded = false;
var toLoad = [];
function waitLoad(func, _this, args) {
	if (isLoaded) return (false);
	toLoad.push({ "func": func, "this": _this, "args": args });
	return (true);
}

function endLoad(evt) {
	isLoaded = true;
	evt.target.parentNode.style.display = "none";
	svgDice = evt.target.parentNode.appendChild(evt.target.contentDocument.querySelector("svg").cloneNode(true));
	evt.target.parentNode.removeChild(evt.target);
	while (toLoad.length) {
		var f = toLoad.shift();
		f.func.apply(f.this, f.args);
	}
}

var waitInitStorage = { init: [], msg: [] };
function waitInit(isInitMsg, func, _this, args) {
	if (currentMap) return(false);
	var obj = { "func": func, "this": _this, "args": args };
	if (isInitMsg) waitInitStorage.init.push(obj);
	else waitInitStorage.msg.push(obj);
	return (true);
}

function endInit() {
	if (!currentMap) return (false);

	while (waitInitStorage.init.length) {
		var f = waitInitStorage.init.shift();
		f.func.apply(f.this, f.args);
	}
	while (waitInitStorage.msg.length) {
		var f = waitInitStorage.msg.shift();
		f.func.apply(f.this, f.args);
	}
}

const strokePoints = [[3, 4], [4, 5], [2, 3], [0, 5], [1, 2], [0, 1]];
var svgContent = null;
var svgDice = null;
var mapContent = null;
var playerContent = null;
var infoContent = null;
var winnerContent = null;
function initMap() {
	initGrid(currentMap.width, currentMap.height);

	for (var r in currentMap.regions) {
		var nb = 0;
		currentMap.regions[r].center = { x: 0, y: 0 };
		var regionObj = document.createElementNS('http://www.w3.org/2000/svg', 'g');
		regionObj.setAttribute('id', "region_" + r);
		currentMap.regions[r].obj = regionObj = mapContent.querySelector("#map").appendChild(regionObj);
		regionObj.setAttribute("stroke-opacity", "0.1");
		regionObj.setAttribute("fill", "#AAA");
		var regionAnim = document.createElementNS('http://www.w3.org/2000/svg', 'animate');
		currentMap.regions[r].anim = regionAnim;
		setAttributes(regionAnim, {
			"id": "anim",
			"attributeName": "fill-opacity",
			"values": "0.7 ; 0 ; 0.7 ; 0 ; 0.7",
			"keyTimes": "0 ; 0.25 ; 0.5 ; 0.75 ; 1",
			"dur": "1.5s",
			"begin": "none"
		});
		regionObj.appendChild(regionAnim);

		for (var c in currentMap.regions[r]) {
			var cell = currentMap.regions[r][c];
			var cellObj = document.getElementById('cell_' + cell.x + '_' + cell.y);
			if (!cellObj) continue;
			cellObj = regionObj.appendChild(cellObj.parentNode.removeChild(cellObj));
			cellObj.region = (parseInt(r) + 1);
			currentMap.regions[r].center.x += parseInt(cellObj.getAttribute('_x'));
			currentMap.regions[r].center.y += parseInt(cellObj.getAttribute('_y'));
			++nb;
		}
		if (nb != 0) {
			currentMap.regions[r].center.x /= nb;
			currentMap.regions[r].center.y /= nb;
		}
	}

	for (var r in currentMap.regions) {
		for (var c in currentMap.regions[r]) {
			var cell = currentMap.regions[r][c];
			var cellObj = document.getElementById('cell_' + cell.x + '_' + cell.y);
			if (!cellObj) continue;
			cellObj.parsed = true;

			var i = 0;
			for (var y = cell.y - 1; y < cell.y + 2; ++y) {
				for (var _x = 0; _x < 2; ++_x) {
					var x = cell.x - 1;
					if (y == cell.y) x += _x * 2;
					else x += _x + (y + 1) % 2;

					var draw = false;
					if (x < 0 || y < 0 || x >= currentMap.width || y >= currentMap.height) draw = true;
					else {
						var cellObj2 = document.getElementById('cell_' + x + '_' + y);
						if (cellObj2 && !cellObj2.parsed && cellObj2.region != cellObj.region) draw = true;
					}

					if (draw) {
						var line = document.createElementNS('http://www.w3.org/2000/svg', 'line');
						setAttributes(line, {
							'x1': cellObj.data[strokePoints[i][0]].x,
							'y1': cellObj.data[strokePoints[i][0]].y,
							'x2': cellObj.data[strokePoints[i][1]].x,
							'y2': cellObj.data[strokePoints[i][1]].y,
							"stroke": "black"
						});
						mapContent.querySelector("#map").appendChild(line);
					}

					++i;
				}
			}
		}
	}
}

const cellSize = { w: 30, h: 20 };
const gameOffset = { x: 10, y: 30 };
var gameSize = { w: 0, h: 10 };
function initGrid(width, height) {
	var content = document.getElementById('content');
	if (!content) return (false);

	content.innerHTML = "";
	var svgDiceDiv = document.createElement("div");
	svgDiceDiv = content.appendChild(svgDiceDiv);
	svgDice = document.createElement("object");
	svgDice.onload = endLoad;
	setAttributes(svgDice, {
		"id": "svgdice",
		"type": "image/svg+xml",
		"data": "img/dice.svg",
		"display": "none"
	});
	svgDice = svgDiceDiv.appendChild(svgDice);

	svgContent = document.createElementNS("http://www.w3.org/2000/svg", "svg");
	setAttributes(svgContent, {
		"width": "100%",
		"height": "100%"
	});

	mapContent = document.createElementNS("http://www.w3.org/2000/svg", "g");
	mapContent.setAttribute("id", "mapcontent");
	var mapNode = document.createElementNS("http://www.w3.org/2000/svg", "g");
	setAttributes(mapNode, {
		"id": "map",
		"fill": "#EEE",
		"stroke": "#DDD"
	});
	mapContent.appendChild(mapNode);
	var stateNode = document.createElementNS("http://www.w3.org/2000/svg", "g");
	stateNode.setAttribute("id", "state");
	mapContent.appendChild(stateNode);
	svgContent.appendChild(mapContent);

	var offsetx;
	for (var i = 0; i < width; ++i) {
		for (var j = 0; j < height; ++j) {
			if (j % 2) offsetx = gameOffset.x + cellSize.w / 2.0;
			else offsetx = gameOffset.x;
			var _x = i * cellSize.w + cellSize.w / 2.0;
			var _y = j * (cellSize.h * (1 - (1 - Math.sin(Math.PI / 6)) / 2)) + cellSize.h / 2.0;
			mapNode.appendChild(genHexagonSVG(_x + offsetx, _y + gameOffset.y, cellSize.w, cellSize.h, 'cell_' + i + '_' + j));

			_x += cellSize.w / 2.0;
			_y += cellSize.h / 2.0
			if (gameSize.w < _x) gameSize.w = _x;
			if (gameSize.h < _y) gameSize.h = _y;
		}
	}

	var infoNodeContent;
	infoContent = document.createElementNS("http://www.w3.org/2000/svg", "g");
	setAttributes(infoContent, {
		"id": "infocontent",
		"transform": "translate(0," + (gameOffset.y + gameSize.h + cellSize.h / 2) + ")"
	});
	infoNodeContent = document.createElementNS("http://www.w3.org/2000/svg", "text");
	setAttributes(infoNodeContent, {
		"id": "text",
		"text-anchor": "end",
		"x": (gameOffset.x + gameSize.w + cellSize.w / 2.0) / 2.0 - cellSize.w * 2,
		"y": cellSize.h * 1.8,
		"font-size": 25,
		"font-family": "Verdana",
		"font-weight": "bold"
	});
	infoContent.appendChild(infoNodeContent);
	infoNodeContent = document.createElementNS("http://www.w3.org/2000/svg", "g");
	setAttributes(infoNodeContent, {
		"id": "dices",
		"transform": "translate(" + ((gameOffset.x + gameSize.w + cellSize.w / 2.0) / 2.0) + "," + (2 * cellSize.h / 3.0) + ")"
	});
	infoContent.appendChild(infoNodeContent);
	svgContent.appendChild(infoContent);

	playerContent = document.createElementNS("http://www.w3.org/2000/svg", "g");
	playerContent.setAttribute("id", "playercontent");
	svgContent.appendChild(playerContent);

	var winnerTextContent, defNode, node, subnode;
	winnerContent = document.createElementNS("http://www.w3.org/2000/svg", "g");
	winnerContent.setAttribute("transform", "translate(" + (gameOffset.x + gameSize.w / 2) + "," + (gameOffset.y + gameSize.h / 2) + ")");
	defNode = document.createElementNS("http://www.w3.org/2000/svg", "defs");
	node = document.createElementNS("http://www.w3.org/2000/svg", "radialGradient");
	node.setAttribute("id", "winnerGradiant");
	subnode = document.createElementNS("http://www.w3.org/2000/svg", "stop");
	setAttributes(subnode, {
		"offset": "0%",
		"style": "stop-color:rgb(255,255,0);stop-opacity:0.6;"
	});
	node.appendChild(subnode);
	subnode = document.createElementNS("http://www.w3.org/2000/svg", "stop");
	setAttributes(subnode, {
		"offset": "70%",
		"style": "stop-color:rgb(255,255,0);stop-opacity:0.4;"
	});
	node.appendChild(subnode);
	subnode = document.createElementNS("http://www.w3.org/2000/svg", "stop");
	setAttributes(subnode, {
		"offset": "100%",
		"style": "stop-color:rgb(255,255,255);stop-opacity:0;"
	});
	node.appendChild(subnode);
	defNode.appendChild(node);
	winnerContent.appendChild(defNode);
	node = document.createElementNS("http://www.w3.org/2000/svg", "ellipse");
	setAttributes(node, {
		"cx": "0",
		"cy": "0",
		"rx": (gameOffset.x + gameSize.w / 2),
		"ry": (gameOffset.y + gameSize.h / 2),
		"style": "fill:url(#winnerGradiant)"
	});
	winnerContent.appendChild(node);
	winnerTextContent = document.createElementNS("http://www.w3.org/2000/svg", "text");
	setAttributes(winnerTextContent, {
		"id": "winMessage",
		"y": "-30",
		"dominant-baseline": "middle",
		"text-anchor": "middle",
		"font-size": "60",
		"style": "text-shadow: 4px 4px 10px grey; font-weight: 900; font-family: Verdana;"
	});
	winnerContent.appendChild(winnerTextContent);
	winnerTextContent = document.createElementNS("http://www.w3.org/2000/svg", "text");
	setAttributes(winnerTextContent, {
		"id": "winPName",
		"y": "30",
		"dominant-baseline": "middle",
		"text-anchor": "middle",
		"font-size": "40",
		"fill": "rgb(120,0,0)",
		"style": "text-shadow: 4px 4px 10px grey; font-weight: 900; font-family: Verdana;"
	});
	winnerContent.appendChild(winnerTextContent);
	winnerContent.setAttribute("visibility", "hidden");
	svgContent.appendChild(winnerContent);

	content.appendChild(svgContent);
}

function setWinner(id) {
	var node;
	var player;

	if (Players.length <= id) return;
	player = Players[id];

	node = winnerContent.querySelector("#winMessage");
	if (node) node.textContent = "Vainqueur Joueur #" + (id + 1) + " !";

	node = winnerContent.querySelector("#winPName");
	if (node) node.textContent = player.name;

	winnerContent.setAttribute("visibility", "visible");
}

function hideWinner() {
	winnerContent.setAttribute("visibility", "hidden");
}

function genHexagonSVG(x, y, w, h, id) {
	var node = document.createElementNS('http://www.w3.org/2000/svg', 'polygon');

	var val = "";
	node.data = [];

	w /= Math.cos(Math.PI / 6);
	for (var i = 0; i < 6; ++i) {
		var angle = Math.PI / 3 * i + Math.PI / 6;
		if (i) val += " ";
		var obj = { x: Math.cos(angle) * w / 2 + x, y: Math.sin(angle) * h / 2 + y };
		val += obj.x + "," + obj.y;
		node.data.push(obj);
	}
	setAttributes(node, {
		"points": val,
		'id': id,
		'_x': x,
		'_y': y
	});

	return (node);
}

function setRegionData(id, color, dices) {
	if (waitLoad(setRegionData, this, arguments)) return;

	var stateObj = document.getElementById('state');
	if (!stateObj) return;
	var region = currentMap.regions[id];
	if (!region) return;

	setAttributes(region.obj, {
		"fill": color,
		"fill-opacity": "0.7"
	});

	var diceObj = stateObj.querySelector("#dice_" + id);
	if (!diceObj) {
		diceObj = document.createElementNS('http://www.w3.org/2000/svg', 'use');
		//diceObj = svgDice.contentDocument.getElementById('dicef6')
		setAttributes(diceObj, {
			"id": "dice_" + id,
			"x": region.center.x,
			"y": region.center.y
		});
		stateObj.appendChild(diceObj);
	}
	setAttributes(diceObj, {
		"href": "#dice" + dices,
		"fill": color
	});
}

var Players = [];
const PlayersColor = ["#b180ff", "#f55654", "#fefe00", "#2b9300", "#baff00", "#bcffff", "#f87fff", "#f67e00"];
function initPlayer(player) {
	if (waitInit(true, initPlayer, this, arguments)) return;

	if (!"id" in player) return;
	Players[player.id] = { ...player };
	drawPlayers();
}

const minPlayerSize = 200;
const minPlayerSpace = cellSize.w;
function drawPlayers() {
	if (waitLoad(drawPlayers, this, arguments)) return;
	if (!Players) return;
	if (!playerContent) return;

	var nb = 0;
	for (var i in Players) {
		var playerContener = document.getElementById('player_' + i);
		var node, subnode;
		if (!playerContener) {
			playerContener = document.createElementNS('http://www.w3.org/2000/svg', 'g');
			playerContener.setAttribute("id", 'player_' + i);
			playerContent.appendChild(playerContener);

			node = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
			setAttributes(node, {
				"height": cellSize.h * 4,
				"fill": "#CCC",
				"stroke": "#999",
				"rx": cellSize.w / 3.0,
				"ry": cellSize.w / 3.0
			});
			playerContener.appendChild(node);

			node = document.createElementNS('http://www.w3.org/2000/svg', 'text');
			node.textContent = "#" + (nb + 1);
			setAttributes(node, {
				"font-family": "Verdana",
				"x": 10,
				"y": 20
			});
			playerContener.appendChild(node);

			node = document.createElementNS('http://www.w3.org/2000/svg', 'g');
			node.setAttribute('transform', 'translate(30,45)');
			subnode = document.createElementNS('http://www.w3.org/2000/svg', 'use');
			setAttributes(subnode, {
				"href": "#dice",
				"transform": "scale(0.7,0.7)",
				"fill": PlayersColor[i]
			});
			node.appendChild(subnode);
			subnode = document.createElementNS('http://www.w3.org/2000/svg', 'text');
			setAttributes(subnode, {
				"id": "score",
				"font-size": 25,
				"font-family": "Verdana",
				"font-weight": "bold",
				"x": 25,
				"y": 14
			});
			subnode.textContent = "0";
			node.appendChild(subnode);
			subnode = document.createElementNS('http://www.w3.org/2000/svg', 'use');
			setAttributes(subnode, {
				"href": "img/treasure.svg#treasure",
				"transform": "scale(0.35,0.35)",
				"x": 300,
				"fill": PlayersColor[i]
			});
			node.appendChild(subnode);
			subnode = document.createElementNS('http://www.w3.org/2000/svg', 'text');
			setAttributes(subnode, {
				"id": "stock",
				"font-size": 25,
				"font-family": "Verdana",
				"font-weight": "bold",
				"x": 130,
				"y": 14
			});
			subnode.textContent = "0";
			node.appendChild(subnode);
			playerContener.appendChild(node);

			node = document.createElementNS('http://www.w3.org/2000/svg', 'g');
			node.setAttribute("id", "player_data");
			playerContener.appendChild(node);
		}
		var _w = gameSize.w + cellSize.w / 2.0;
		var _n = Players.length;
		var size = (_w - (_n - 1) * minPlayerSpace) / _n;
		if (minPlayerSize > size) {
			var _n = Math.floor((_w + minPlayerSpace) / (minPlayerSize + minPlayerSpace));
			size = (_w - (_n - 1) * minPlayerSpace) / _n;
		}
		var _x = gameOffset.x + (nb % _n) * (size + minPlayerSpace);
		var _y = gameOffset.y + cellSize.h * 3 + gameSize.h + cellSize.h + Math.floor(nb / _n) * (cellSize.h * 5);
		playerContener.setAttribute('transform', 'translate(' + _x + ',' + _y + ')');
		playerContener.querySelector("rect").setAttribute("width", size);

		var playerData = playerContener.querySelector("#player_data");
		updatePlayerContent(playerContener, Players[i]);

		nb++;
	}
}

function updatePlayerContent(contener, player) {
	if (!contener instanceof SVGElement) contener = document.getElementById('player_' + contener);
	if (!contener) return;

	var node;

	// Set text
	if (!(node = contener.querySelector("#name"))) {
		node = document.createElementNS('http://www.w3.org/2000/svg', 'text');
		setAttributes(node, {
			'id': 'name',
			"font-family": "Verdana",
			'font-style': 'italic',
			"font-weight": "bold",
			'x': '40',
			'y': '20',
			'lengthAdjust': 'spacingAndGlyphs'
		});
		contener.appendChild(node);
	}
	node.textContent = player.name;
}

var GameStates = [];
var CurGameState = 0;
function setState(gameState) {
	if (waitInit(false, setState, this, arguments)) return;

	var turn = parseInt(gameState.id);
	GameStates[turn] = gameState;
	if (CurGameState <= turn) {
		displayState(turn);
	}
}

var CurrentStateChange = false;
var NextStateChange = [];
async function displayState(turn, details = true) {
	if (waitLoad(displayState, this, arguments)) return;
	NextStateChange.push({ "turn": turn, "details": details });
	if (CurrentStateChange) return;
	CurrentStateChange = true;

	do {
		var args = NextStateChange.shift();
		await _displayState(args["turn"], args["details"]);
	} while (NextStateChange.length);
	CurrentStateChange = false;
}

async function _displayState(turn, details) {
	if (!infoContent) return;

	var newGameState = GameStates[turn];
	var oldGameState = turn != CurGameState ? GameStates[CurGameState] : null;
	if (!newGameState) return;

	if (typeof newGameState.winner !== 'undefined') {
		setWinner(newGameState.winner);
	} else {
		hideWinner();
    }

	if (details && newGameState.turn && typeof newGameState.turn.player !== 'undefined') {
		infoContent.querySelector("#text").textContent = "Joueur #" + (newGameState.turn.player + 1);
		var diceContent = infoContent.querySelector("#dices");
		var node;
		while (node = diceContent.firstChild) node.remove();
		var offset = 0;
		var player = [newGameState.turn.attack.from, newGameState.turn.attack.to];
		var region;
		for (var p = 0; p < 2; ++p) {
			var score = 0;
			for (var i in newGameState.turn.dices[p]) {
				var dice = newGameState.turn.dices[p][i];
				if (dice > 0) {
					score += dice;
					var subnode = document.createElementNS('http://www.w3.org/2000/svg', 'use');
					setAttributes(subnode, {
						"href": "#dicef" + dice,
						"transform": "translate(" + offset + ",0) scale(1,1)",
						"fill": PlayersColor[oldGameState.map[player[p]].owner]
					});
					diceContent.appendChild(subnode);
					offset += 35;
				}
			}
			var subnode = document.createElementNS('http://www.w3.org/2000/svg', 'text');
			setAttributes(subnode, {
				"font-size": "18",
				"font-family": "Verdana",
				'font-style': 'italic',
				"transform": "translate(" + (offset + 5) + ",20)",
				"fill": PlayersColor[oldGameState.map[player[p]].owner]
			});
			subnode.textContent = score;
			diceContent.appendChild(subnode);
			offset += 60;
			region = document.getElementById("region_" + player[p]);
			if (region) region.querySelector("#anim").beginElement();
		}
	}
	else {
		infoContent.querySelector("#text").textContent = "";
		var diceContent = infoContent.querySelector("#dices");
		var node;
		while (node = diceContent.firstChild) node.remove();
	}

	for (var i in newGameState.stock) {
		var playerContener = document.getElementById('player_' + i);
		if (!playerContener) continue;
//		if (!oldGameState || (newGameState.points[i] != oldGameState.points[i])) {
			var scoreObj = playerContener.querySelector("#score");
			scoreObj.textContent = newGameState.points[i];
//		}
//		if (!oldGameState || (newGameState.stock[i] != oldGameState.stock[i])) {
			var stockObj = playerContener.querySelector("#stock");
			stockObj.textContent = newGameState.stock[i];
//		}
	}

	for (var i in newGameState.map) {
		if (!oldGameState || (newGameState.map[i].dices != oldGameState.map[i].dices) || (newGameState.map[i].owner != oldGameState.map[i].owner)) {
			setRegionData(i, PlayersColor[newGameState.map[i].owner], newGameState.map[i].dices);
		}
	}
	CurGameState = turn;
	await sleep(1500);
}

function sleep(ms) {
	return new Promise(resolve => setTimeout(resolve, ms));
}
