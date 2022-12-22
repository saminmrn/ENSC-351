"use strict";
// Client-side interactions with the browser.

// Make connection to server when web page is fully loaded.
var socket = io.connect();


function getUpdatedHR() {
	var hr_monitor = setInterval(function() {
		sendPrimeCommand("hr_monitor", "getHeartRate\n");
	}, 10000);
}

function getUpdatedSteps() {
	var steps_monitor = setInterval(function() {
		sendPrimeCommand("steps_monitor", "getSteps\n");
	}, 5000);
}

function getUpdatedCal() {
	var cal_monitor = setInterval(function() {
		sendPrimeCommand("calories_monitor", "getCalories\n");
	}, 15000);
}

function getUpdatedDistance() {
	var distance_monitor = setInterval(function() {
		sendPrimeCommand("distance_monitor", "getDistance\n");
	}, 10000);
}

$(document).ready(function() {

	getUpdatedCal();
	getUpdatedDistance();
	getUpdatedSteps();
	getUpdatedHR();

	$('#submit').click(function() {
		var weight = $('#weightForm').val();
		var setWeight = "setWeight" + " " + weight;
		sendPrimeCommand("prime", setWeight);
		$('#weight').html(weight+"lb");

		var height = $('#heightForm').val();
		var setHeight = "setHeight" + " " + height;
		$('#height').html(height+"cm");
		sendPrimeCommand("prime", setHeight);

	});	

	$('#displaySteps').click(function() {
		sendPrimeCommand("prime", "displaySteps\n");
	})

	$('#displayHeartRate').click(function() {
		sendPrimeCommand("prime", "displayHeartRate\n");
	})

	$('#setTimer').click(function() {
		sendPrimeCommand("prime", "setTimer\n");
	})

	$('#displayStats').click(function() {
		sendPrimeCommand("prime", "displayStats\n");
	});
	
	socket.on('heartRate_monitor_reply', function(result) {
		updateMonitor("#heartrate", result);
	});

	socket.on('calories_monitor_reply', function(result) {
		updateMonitor("#calories", result);
	});

	socket.on('steps_monitor_reply', function(result) {
		updateMonitor("#steps", result);
	});

	socket.on('distance_monitor_reply', function(result) {
		updateMonitor("#distance", result);
	});
	
});

function sendPrimeCommand(command, message) {
	socket.emit(command, message);
};

function updateMonitor(key, value) {
	$(key).html(value);
}