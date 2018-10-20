(function(i) {
	var json = JSON.parse(i);
	return ((json["settings"]["reverseRelays"]) == true ? "ON" : "OFF");
})(input)


