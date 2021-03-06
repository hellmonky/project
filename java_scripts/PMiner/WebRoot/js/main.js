//require js's main load
/*
require.config({
  baseUrl: "js",
  paths: {
    "jquery": "jquery.min",
    "bootstrap": "bootstrap.min",
    "echarts": "echarts",
    "echarts/chart/pie": "./chart/pie"
  },
  shim: {
    "jquery.newsTicker": {deps: ["jquery"]},
    "bootstrap": {deps: ["jquery"]},
    "semantic": {deps: ["jquery"]}
  }
})
*/

//for js autocomplete plugin
var names = new Bloodhound({
  datumTokenizer: Bloodhound.tokenizers.obj.whitespace('profileName'),
  queryTokenizer: Bloodhound.tokenizers.whitespace,
  limit: 10,
  prefetch: {
    url: 'findAllNameList.action',
    ttl: 30000, // ms
    thumbprint: 'find',
    // the json file contains an array of strings, but the Bloodhound
    // suggestion engine expects JavaScript objects so this converts all of
    // those strings
    filter: function(list) {
      return $.map(list, function(name) { return { profileName: name }; });
    }
  }
});

names.initialize();

$('.typeahead').typeahead(null, {
  name: 'names',
  displayKey: 'profileName',
  source: names.ttAdapter()
});
