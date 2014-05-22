Pebble.addEventListener("ready", function() {
  var phase = 4;
  var room = 839;
  var months = new Array(31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
  var date = new Date();
  var now = date.getDate();
  var month = date.getMonth()+1;
  var url = "http://ets-res"+phase+"-"+room+":ets"+room+"@www2.cooptel.qc.ca/services/temps/?mois="+month+"&cmd=Visualiser";

  var req = new XMLHttpRequest();
  req.open('GET', url, true);
  req.onload = function() {
    if (req.readyState == 4) {
      var maxData = req.responseText.match(/<TD>Quota permis pour la p&eacute;riode<\/TD><TD ALIGN="RIGHT">(.*?)<\/TD><\/TD><\/TR>/)[1]/1024;
      var currentData = req.responseText.match(/<TR><TD COLSPAN="3"><B>Total combin&eacute;:<\/B><\/TD><TD ALIGN="RIGHT">(.*?)<\/TD><\/TR>/)[1]/1024;
      var percentage = currentData/maxData * 100;

      Pebble.sendAppMessage({
        "percentage": "["+percentage.toFixed(2)+"%]",
        "room": "Apt. "+phase+"-"+room,
        "data": currentData.toFixed(2)+"/"+maxData.toFixed(2)+" GB",
        "days": (months[month]-now)+" days left"
      });
    }
  };

  req.send(null);
});

