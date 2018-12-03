/*(function(i) {
	var json = JSON.parse(i);
	return ((json["settings"]["reverseRelays"]) == true ? "ON" : "OFF");
})(input)
*/
result = ((JSON.parse(input)["settings"]["reverseRelays"]) == true ? "ON" : "OFF");
