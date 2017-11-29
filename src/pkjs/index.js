var forceRequest = function (url, type, callback) {
  var forcetitle = new XMLHttpRequest();
    forcetitle.onload = function () {
    //callback(this.responseText);
  };
  forcetitle.open(type, url);
  forcetitle.send();
  };

Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
  }
    );

// Get AppMessage events
Pebble.addEventListener('appmessage', function(e) {
  // Get the dictionary from the message
  var dict = e.payload;
  
  console.log(e.payload.FIRE_TYPE);
  console.log('Got message: ' + JSON.stringify(dict));
  
  switch(e.payload.FIRE_TYPE){
    case 0:
      forceRequest('https://maker.ifttt.com/trigger/high_fire/with/key/PoEGl_9apA1FuIfrIoE3N', 'GET');
      console.log('MEKALECA HIGH!');
    break;
    case 1:
      forceRequest('https://maker.ifttt.com/trigger/high_fire/with/key/PoEGl_9apA1FuIfrIoE3N', 'GET');
    break;
    case 2:
      forceRequest('https://maker.ifttt.com/trigger/low_fire/with/key/PoEGl_9apA1FuIfrIoE3N', 'GET');
      console.log('MEKA HINEY HO!');
    break;
  }
});