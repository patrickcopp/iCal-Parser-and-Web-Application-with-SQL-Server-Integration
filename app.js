'use strict'

// C library API
const ffi = require('ffi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

const mysql = require('mysql');
let connection;



app.use(fileUpload());

var bodyParser  = require('body-parser')
app.use(bodyParser.urlencoded({
  extended: true
}));
app.use(express.json());       // to support JSON-encoded bodies
app.use(bodyParser.json());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }

  let uploadFile = req.files.uploadFile;
  if(uploadFile.name.substring(uploadFile.name.length-4)!=".ics")
  {
    return res.status(400).send('Not a .ics file.')
  }


  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      res.send('');
    }
  });
});

//******************** Your code goes here ********************



//Sample endpoint
app.get('/patarkPoint', function(req , res){
  let libcal = ffi.Library('./libcal', {
  'testFunction':['string',[]],
  });


  let res2 = libcal.testFunction();

  res.send(res2);
});

app.get('/pageLoadFiles', function(req , res){


  let hold = new Array();
  let libcal = ffi.Library('./libcal', {
  'fileToJSON':['string',['string']],
  });
  let hi=true;
  fs.readdirSync("./uploads/").forEach(file => {
    hi=true;
    let toReturn=libcal.fileToJSON('uploads/'+file);
    if(toReturn=="{}")
    {
      hi=false;
    }
    let hello=JSON.parse(toReturn);

    if(hi)
    {
      hold.push(hello);
    }

  });
  res.send(hold);
});

app.get('/newCal', function(req , res){

  let libcal = ffi.Library('./libcal', {
  'simpleToCal':['string',['string','string','string','string','string','string']],
  });

  let toReturn= libcal.simpleToCal('uploads/'+req.query.hi.filename,req.query.hi.prodID,req.query.hi.version,req.query.hi.eventUID,req.query.hi.eventDTSTAMP,req.query.hi.eventDTSTART);
  if(toReturn[0]!='{')
  {
    res.send("Calendar created incorrectly");
  }

  res.send(toReturn);
});

app.get('/allFileNames', function(req , res){
  let hold = new Array();
  fs.readdirSync("./uploads/").forEach(file => {
    hold.push(file);
});
  res.send(hold);
});

app.get('/someendpoint', function(req , res){

  res.send("test");
});

app.get('/getEvents', function(req , res){
  let libcal = ffi.Library('./libcal', {
  'fileToJSONEvents':['string',['string']],
  });
  fs.readdirSync("./uploads/").forEach(file => {
    if(file==req.query.strUser)
    {
      let hi=libcal.fileToJSONEvents("uploads/"+file);
      let toReturn=JSON.parse(libcal.fileToJSONEvents("uploads/"+file));
      res.send(toReturn);
    }
  });
});

app.get('/newEvent', function(req , res){
  let libcal = ffi.Library('./libcal', {
  'fileToNewEvent':['string',['string','string','string','string','string']],
  });
  if(req.query.newDTSTART.length!=15 && req.query.newDTSTART.length!=16)
  {
    res.send("INVALID DT");
  }
  if(req.query.newDTSTAMP.length!=15 && req.query.newDTSTAMP.length!=16)
  {
    res.send("INVALID DT");
  }
  let ok=libcal.fileToNewEvent("uploads/"+req.query.strUser2,req.query.newUid,req.query.newSUMMARY,req.query.newDTSTART,req.query.newDTSTART);

  res.send(ok);
});

app.get('/allAlarms', function(req , res){

  let libcal = ffi.Library('./libcal', {
  'fileToAllAlarms':['string',['string','string']],
  });
  let hello=libcal.fileToAllAlarms("uploads/"+req.query.fileName,req.query.eventNumber);
  res.send(hello);
});

app.get('/allProps', function(req , res){

  let libcal = ffi.Library('./libcal', {
  'fileToAllProps':['string',['string','string']],
  });
  let hello=libcal.fileToAllProps("uploads/"+req.query.fileName,req.query.eventNumber);
  res.send(hello);
});

app.get('/clearData', function(req , res){
  let hello="DROP TABLE ALARM";
  connection.query(hello,function (err, rows, fields)
  {
    if (err)
    {
      console.log("Something went wrong. "+err);
    }
  });
  hello="DROP TABLE EVENT";
  connection.query(hello,function (err, rows, fields)
  {
    if (err)
    {
      console.log("Something went wrong. "+err);
    }
  });
  hello="DROP TABLE FILE";
  connection.query(hello,function (err, rows, fields)
  {
    if (err)
    {
      console.log("Something went wrong. "+err);
    }
  });

  hello="CREATE TABLE IF NOT EXISTS FILE (cal_id int AUTO_INCREMENT,file_name varchar(60) NOT NULL, version int NOT NULL, prod_id varchar(256) NOT NULL,PRIMARY KEY(cal_id))";
  connection.query(hello,function (err, rows, fields)
  {
    if (err)
    {
      console.log("Something went wrong. "+err);
    }
  });
  hello="CREATE TABLE IF NOT EXISTS EVENT (event_id int AUTO_INCREMENT,summary varchar(1024), start_time DateTime NOT NULL, location varchar(60),organizer varchar(256),cal_file int NOT NULL, PRIMARY KEY(event_id),FOREIGN KEY(cal_file) REFERENCES FILE(cal_id) ON DELETE CASCADE)";
  connection.query(hello,function (err, rows, fields)
  {
    if (err)
    {
      console.log("Something went wrong. "+err);
    }
  });
  hello="CREATE TABLE IF NOT EXISTS ALARM (alarm_id int AUTO_INCREMENT,action varchar(256) NOT NULL, `trigger` varchar(256) NOT NULL,event int NOT NULL, PRIMARY KEY(alarm_id),FOREIGN KEY(event) REFERENCES EVENT(event_id) ON DELETE CASCADE)";
  connection.query(hello,function (err, rows, fields)
  {
    if (err)
    {
      console.log("Something went wrong. "+err);
    }
  });
});

app.get('/storeFiles', function(req , res){
  let hello="DROP TABLE ALARM";
  connection.query(hello,function (err, rows, fields)
  {
    if (err)
    {
      console.log("Something went wrong. "+err);
    }
  });
  hello="DROP TABLE EVENT";
  connection.query(hello,function (err, rows, fields)
  {
    if (err)
    {
      console.log("Something went wrong. "+err);
    }
  });
  hello="DROP TABLE FILE";
  connection.query(hello,function (err, rows, fields)
  {
    if (err)
    {
      console.log("Something went wrong. "+err);
    }
  });

  hello="CREATE TABLE IF NOT EXISTS FILE (cal_id int AUTO_INCREMENT,file_name varchar(60) NOT NULL, version int NOT NULL, prod_id varchar(256) NOT NULL,PRIMARY KEY(cal_id))";
  connection.query(hello,function (err, rows, fields)
  {
    if (err)
    {
      console.log("Something went wrong. "+err);
    }
  });
  hello="CREATE TABLE IF NOT EXISTS EVENT (event_id int AUTO_INCREMENT,summary varchar(1024), start_time DateTime NOT NULL, location varchar(60),organizer varchar(256),cal_file int NOT NULL, PRIMARY KEY(event_id),FOREIGN KEY(cal_file) REFERENCES FILE(cal_id) ON DELETE CASCADE)";
  connection.query(hello,function (err, rows, fields)
  {
    if (err)
    {
      console.log("Something went wrong. "+err);
    }
  });
  hello="CREATE TABLE IF NOT EXISTS ALARM (alarm_id int AUTO_INCREMENT,action varchar(256) NOT NULL, `trigger` varchar(256) NOT NULL,event int NOT NULL, PRIMARY KEY(alarm_id),FOREIGN KEY(event) REFERENCES EVENT(event_id) ON DELETE CASCADE)";
  connection.query(hello,function (err, rows, fields)
  {
    if (err)
    {
      console.log("Something went wrong. "+err);
    }
  });


  let hold = new Array();
  let libcal = ffi.Library('./libcal', {'fileToJSON':['string',['string']],'fileToJSONEvents':['string',['string']],'fileToJSONAlarms':['string',['string','string']],});

  let hi=true;
  fs.readdirSync("./uploads/").forEach(file => {
    hi=true;
    let toReturn=libcal.fileToJSON('uploads/'+file);
    if(toReturn=="{}")
    {
      hi=false;
    }
    if(hi)
    {
      let hello=JSON.parse(toReturn);
      let queryInsert="INSERT INTO FILE(file_name,version,prod_id) VALUES(\""+hello.filename+"\","+hello.version+",\""+hello.prodID+"\")";

      connection.query(queryInsert,function (err, rows, fields)
      {
        if (err)
        {
          console.log("Something went wrong. "+err);
          return "";
        }
        let toReturn=libcal.fileToJSONEvents('uploads/'+file);
        let hold=JSON.parse(toReturn);

        for(let i=0;i<Object.keys(hold).length;i++)
        {

          let newDate=hold[i]["startDT"].date.substring(0,4)+"-"+hold[i]["startDT"].date.substring(4,6)+"-"+hold[i]["startDT"].date.substring(6,8)+" "+hold[i]["startDT"].time.substring(0,2)+":"+hold[i]["startDT"].time.substring(2,4)+":"+hold[i]["startDT"].time.substring(4,6);

          let queryInsert2="INSERT INTO EVENT(summary,start_time,location,organizer,cal_file) VALUES(\""+hold[i]["summary"]+"\",\""+newDate+"\",\"\",\"\","+rows.insertId+")";
          connection.query(queryInsert2,function (err, rows2, fields)
          {
            if (err)
            {
              console.log("Something went wrong events. "+err);
              return "";
            }
            let n=i.toString();
            let alarmPush=JSON.parse(libcal.fileToJSONAlarms('uploads/'+file,n));
            for(let i=0;i<Object.keys(alarmPush).length;i++)
            {
              let queryInsert2="INSERT INTO ALARM(action,`trigger`,event) VALUES(\""+alarmPush[i]["action"]+"\",\""+alarmPush[i]["trigger"]+"\","+rows2.insertId+")";
              connection.query(queryInsert2,function (err, rows, fields)
              {
                if (err)
                {
                  console.log("Something went wrong. "+err);
                }
              });
            }


          });
        }



      });
    }


  });



});

app.get('/dbStatus', function(req , res){
  let hello="SELECT COUNT(*) FROM FILE";

  connection.query(hello,function (err, rows, fields)
  {
    if (err)
    {
      console.log("Something went wrong. "+err);
      res.send("Database connection failed");
    }
    let fileCount=rows[0]["COUNT(*)"];
    hello="SELECT COUNT(*) FROM EVENT";

    connection.query(hello,function (err, rows, fields)
    {
      if (err)
      {
        console.log("Something went wrong. "+err);
        res.send("Database connection failed");
      }
      let eventCount=rows[0]["COUNT(*)"];
      hello="SELECT COUNT(*) FROM ALARM";

      connection.query(hello,function (err, rows, fields)
      {
        if (err)
        {
          console.log("Something went wrong. "+err);
          res.send("Database connection failed");
        }
        let alarmCount=rows[0]["COUNT(*)"];
        res.send("Database has "+fileCount+" files, "+eventCount+" events, and "+alarmCount+" alarms");
      });
    });
  });






});

app.get('/displayDate', function(req , res){

  let hello="SELECT * FROM EVENT ORDER BY start_time";

  connection.query(hello,function (err, rows, fields)
  {
    let toReturn = new Array();
    if (err)
    {
      console.log("Something went wrong. "+err);
      res.send("Database connection failed");
    }
    for(let i=0;i<rows.length;i++)
    {
      toReturn.push(rows[i]);
    }
    res.send(toReturn);

  });






});

app.get('/displayEvents', function(req , res){

  let hello="SELECT * FROM FILE WHERE file_name=\"uploads/"+req.query.filename+"\"";

  connection.query(hello,function (err, rows, fields)
  {
    let toReturn = new Array();
    if (err)
    {
      console.log("Something went wrong. "+err);
      res.send("Database connection failed");
    }
    let calID=rows[0]["cal_id"];

    let hello="SELECT * FROM EVENT WHERE cal_file="+calID;
    connection.query(hello,function (err, rows2, fields)
    {
      if (err)
      {
        console.log("Something went wrong. "+err);
        res.send("Database connection failed");
      }
      for(let i=0;i<rows2.length;i++)
      {
        toReturn.push(rows2[i]);
      }
      console.log(toReturn);
      res.send(toReturn);
    });


  });
});


app.get('/conflicts', function(req , res){

  let hello="SELECT * FROM EVENT";
  connection.query(hello,function (err, rows, fields)
  {
    let toReturn = new Array();
    if (err)
    {
      console.log("Something went wrong. "+err);
      res.send("Database connection failed");
    }

    for(let i=0;i<rows.length;i++)
    {
      for(let i2=0;i2<rows.length;i2++)
      {
        if(i!=i2 && rows[i]["start_date"]==rows[i2]["start_date"])
        {
          toReturn.push(rows[i]);
          break;
        }
      }
    }
    res.send(toReturn);


  });
});

app.get('/versionNumber', function(req , res){

  let hello="SELECT * FROM FILE WHERE VERSION="+req.query.number;
  connection.query(hello,function (err, rows, fields)
  {
    let toReturn = new Array();
    if (err)
    {
      console.log("Something went wrong. "+err);
      res.send("Database connection failed");
    }

    for(let i=0;i<rows.length;i++)
    {
      toReturn.push(rows[i]);
    }
    res.send(toReturn);


  });
});

app.get('/alarmConflicts', function(req , res){

  let hello="SELECT * FROM ALARM";
  connection.query(hello,function (err, rows, fields)
  {
    let toReturn = new Array();
    if (err)
    {
      console.log("Something went wrong. "+err);
      res.send("Database connection failed");
    }

    for(let i=0;i<rows.length;i++)
    {
      for(let i2=0;i2<rows.length;i2++)
      {
        if(i!=i2 && rows[i]["trigger"]==rows[i2]["trigger"])
        {
          toReturn.push(rows[i]);
          break;
        }
      }
    }
    res.send(toReturn);


  });
});

app.get('/alarmsIn', function(req , res){

  let hello="SELECT * FROM EVENT e LEFT JOIN ALARM a ON e.event_id = a.event";
  connection.query(hello,function (err, rows, fields)
  {
    let toReturn = new Array();
    if (err)
    {
      console.log("Something went wrong. "+err);
      res.send("Database connection failed");
    }

    for(let i=0;i<rows.length;i++)
    {
      if(rows[i]["alarm_id"]!=null)
      {
        toReturn.push(rows[i]);
      }

    }

    res.send(toReturn);


  });
});

app.get('/login', function(req , res){
  connection = mysql.createConnection({
      host     : 'dursley.socs.uoguelph.ca',
      user     : req.query.hi.username,
      password : req.query.hi.password,
      database : req.query.hi.database
  });
  let toReturn="OK";
  connection.connect(function(err){
    if(err)
    {
      toReturn="FAILED";
    }
    res.send(toReturn);
  });



});






app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
