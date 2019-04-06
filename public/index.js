// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX Example
    //Creates table of calendars
    $('#databaseStuff').hide();
    $.ajax({
        type: 'get',            //Request type
               //Data type - we will use JSON for almost everything
        url: '/pageLoadFiles',   //The server endpoint we are connecting to
        success: function (data) {

          var tbl=$("<table/>").attr("id","mytable");

          $("#fileView").append(tbl);
          var tr="<tr>";
          var td1="<td>File Name</td>";
          var td2="<td>Version</td>";
          var td3="<td>Number of Properties</td>";
          var td4="<td>Number of Events</td>";
          var td5="<td>Product ID</td></tr>";

          $("#mytable").append(tr+td1+td2+td3+td4+td5);

          for(var i=0;i<data.length;i++)
          {
            var tr="<tr>";
            var td1="<td> <a href=\""+data[i]["filename"]+"\" download>"+data[i]["filename"]+"</td>";
            var td2="<td>"+data[i]["version"]+"</td>";
            var td3="<td>"+data[i]["numProps"]+"</td>";
            var td4="<td>"+data[i]["numEvents"]+"</td>";
            var td5="<td>"+data[i]["prodID"]+"</td></tr>";

            $("#mytable").append(tr+td1+td2+td3+td4+td5);

          }
          $("#mytable").attr("border","1");


        },
        error: function(jqXHR, textStatus, errorThrown) {
        }
    });
    //Creates lists of calendars
    $.ajax({
        type: 'get',            //Request type
               //Data type - we will use JSON for almost everything
        url: '/allFileNames',   //The server endpoint we are connecting to
        success: function (data) {
          let select = document.getElementById('calViewSelect');
          let select2 = document.getElementById('eventViewSelect');

          for(let i=0;i<data.length;i++)
          {
            var opt=document.createElement('option');
            opt.value=data[i];
            opt.innerHTML=data[i];
            select.add(opt);
          }
          for(let i=0;i<data.length;i++)
          {
            var opt=document.createElement('option');
            opt.value=data[i];
            opt.innerHTML=data[i];
            select2.add(opt);
          }

        },
        error: function(jqXHR, textStatus, errorThrown) {

        }
    });
    //populates event table with first calendar
    $.ajax({
        type: 'get',            //Request type
        dataType: 'text',       //Data type - we will use JSON for almost everything
        url: '/someendpoint',   //The server endpoint we are connecting to
        success: function (data) {
          var e = document.getElementById('calViewSelect');
          var strUser = e.options[e.selectedIndex].value;

          $.ajax({
              type: 'get',            //Request type
              dataType: 'json',       //Data type - we will use JSON for almost everything
              url: '/getEvents',
              data: {strUser},   //The server endpoint we are connecting to
              success: function (data) {
                var tbl=$("<table/>").attr("id","mytable2");

                $("#calView2").append(tbl);
                var tr="<tr>";
                var td1="<td>Event No.</td>";
                var td2="<td>Start Date</td>";
                var td3="<td>Start Time</td>";
                var td4="<td>Summary</td>";
                var td5="<td>Num Props</td>";
                var td6="<td>Num Alarms</td></tr>";
                $("#mytable2").append(tr+td1+td2+td3+td4+td5+td6);

                for(let i=0;i<Object.keys(data).length;i++)
                {
                  let date=data[i]["startDT"].date.toString().substring(0,4)+"/"+data[i]["startDT"].date.toString().substring(4,6)+"/"+data[i]["startDT"].date.toString().substring(6,8);
                  let time=data[i]["startDT"].time.toString().substring(0,2)+":"+data[i]["startDT"].time.toString().substring(2,4)+":"+data[i]["startDT"].time.toString().substring(4,6);
                  if(data[i]["startDT"].UTC==true)
                  {
                    time.append(" (UTC)");
                  }
                  var tr="<tr>";
                  var td1="<td>"+(i+1)+"</td>";
                  var td2="<td>"+date+"</td>";
                  var td3="<td>"+time+"</td>";
                  var td4="<td>"+data[i]["summary"]+"</td>";
                  var td5="<td>"+data[i]["numProps"]+"</td>";
                  var td6="<td>"+data[i]["numAlarms"]+"</td></tr>";

                  $("#mytable2").append(tr+td1+td2+td3+td4+td5+td6);
                }
                $("#mytable2").attr("border","1");

                let select = document.getElementById('eventChoice');

                for(let i=0;i<Object.keys(data).length;i++)
                {
                  var opt=document.createElement('option');
                  opt.value=data[i];
                  opt.innerHTML=i+1;
                  select.add(opt);
                }
              },
              error: function(jqXHR, textStatus, errorThrown) {
                  // Non-200 return, do something with error
                  alert(errorThrown);
              }
          });


        },
        error: function(jqXHR, textStatus, errorThrown) {
            // Non-200 return, do something with error
            alert(errorThrown);
        }
    });

    // Event listener form replacement example, building a Single-Page-App, no redirects if possible


    $('#buttonClear').click(function(e){
        $('#statView').html("");
        e.preventDefault();

    });

    $('#showAlarmsButton').click(function(e){
      let eventNumber=$("#eventChoice option:selected" ).text();
      let fileName=$("#calViewSelect option:selected" ).text();
      $.ajax({
          type: 'get',            //Request type
          data: {eventNumber,fileName},       //Data type - we will use JSON for almost everything
          url: '/allAlarms',   //The server endpoint we are connecting to
          success: function (data) {
            alert(data);

          },
          error: function(jqXHR, textStatus, errorThrown) {

          }
      });
        $('#statView').append("Show alarms button pressed.<br>");
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the
        //$.ajax({});
    });
    $('#showEventPropsButton').click(function(e){
      let eventNumber=$("#eventChoice option:selected" ).text();
      let fileName=$("#calViewSelect option:selected" ).text();
      $.ajax({
          type: 'get',            //Request type
          data: {eventNumber,fileName},       //Data type - we will use JSON for almost everything
          url: '/allProps',   //The server endpoint we are connecting to
          success: function (data) {
            alert(data);

          },
          error: function(jqXHR, textStatus, errorThrown) {

          }
      });

        $('#statView').append("Show Events properties button pressed.<br>");
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the
        //$.ajax({});
    });
    $('#newEvent').submit(function(e){

      let e2 = document.getElementById('eventViewSelect');
      let strUser2 = e2.options[e2.selectedIndex].value;
      let newUid=document.getElementById('UID').value;
      let newDTSTART=document.getElementById('DTSTART').value;
      let newDTSTAMP=document.getElementById('DTSTAMP').value;
      let newSUMMARY=document.getElementById('SUMMARY').value;
      //makes new event
      $.ajax({
          type: 'get',            //Request type
          data:{strUser2,newUid,newDTSTART,newDTSTAMP,newSUMMARY},       //Data type - we will use JSON for almost everything
          url: '/newEvent',   //The server endpoint we are connecting to
          success: function (data) {
            if(data!="worked")
            {
              $('#statView').append(data+ " caused event to not be created. <br>");
            }
            else
            {
              $('#statView').append("Event created. <br>");
            }

          },
          error: function(jqXHR, textStatus, errorThrown) {

          }
      });
      //clears and updates calendar table
      $('#fileView').html("");
      $.ajax({
          type: 'get',            //Request type
                 //Data type - we will use JSON for almost everything
          url: '/pageLoadFiles',   //The server endpoint we are connecting to
          success: function (data) {
            var tbl=$("<table/>").attr("id","mytable");

            $("#fileView").append(tbl);
            var tr="<tr>";
            var td1="<td>File Name</td>";
            var td2="<td>Version</td>";
            var td3="<td>Number of Properties</td>";
            var td4="<td>Number of Events</td>";
            var td5="<td>Product ID</td></tr>";

            $("#mytable").append(tr+td1+td2+td3+td4+td5);

            for(var i=0;i<data.length;i++)
            {
              var tr="<tr>";
              var td1="<td> <a href=\""+data[i]["filename"]+"\" download>"+data[i]["filename"]+"</td>";
              var td2="<td>"+data[i]["version"]+"</td>";
              var td3="<td>"+data[i]["numProps"]+"</td>";
              var td4="<td>"+data[i]["numEvents"]+"</td>";
              var td5="<td>"+data[i]["prodID"]+"</td></tr>";

              $("#mytable").append(tr+td1+td2+td3+td4+td5);

            }
            $("#mytable").attr("border","1");


          },
          error: function(jqXHR, textStatus, errorThrown) {
          }
      });


      selectMenu();
      e.preventDefault();

    });
    $('#newCalendar').submit(function(e){

      let hi= {version:$('#newCalVersion').val(),filename:fileName=$('#newCalFilename').val(),prodID:$('#newCalprodID').val(),eventUID:$('#newCalEventUID').val(),eventDTSTAMP:$('#newCalEventDTSTAMP').val(),eventDTSTART:$('#newCalEventDTSTART').val()};
      $('#fileView').html("");
      //Creates file
      $.ajax({
          type: 'get',
          data: {
            hi
          },    //Request type
          dataType: 'text',       //Data type - we will use JSON for almost everything
          url: '/newCal',
             //The server endpoint we are connecting to
          success: function (data) {
              if(data[0]=='{')
              {
                $('#statView').append("New Calendar creation attempt: Calendar created successfully.<br>");
              }
              else
              {
                $('#statView').append("New Calendar creation attempt: "+data+".<br>");
              }

          },
          error: function(jqXHR, textStatus, errorThrown) {
              // Non-200 return, do something with error
              alert(errorThrown);
          }
      });
      //updates table of calendars
      $.ajax({
          type: 'get',            //Request type
                 //Data type - we will use JSON for almost everything
          url: '/pageLoadFiles',   //The server endpoint we are connecting to
          success: function (data) {

            var tbl=$("<table/>").attr("id","mytable");

            $("#fileView").append(tbl);
            var tr="<tr>";
            var td1="<td>File Name</td>";
            var td2="<td>Version</td>";
            var td3="<td>Number of Properties</td>";
            var td4="<td>Number of Events</td>";
            var td5="<td>Product ID</td></tr>";

            $("#mytable").append(tr+td1+td2+td3+td4+td5);

            for(var i=0;i<data.length;i++)
            {
              var tr="<tr>";
              var td1="<td> <a href=\""+data[i]["filename"]+"\" download>"+data[i]["filename"]+"</td>";
              var td2="<td>"+data[i]["version"]+"</td>";
              var td3="<td>"+data[i]["numProps"]+"</td>";
              var td4="<td>"+data[i]["numEvents"]+"</td>";
              var td5="<td>"+data[i]["prodID"]+"</td></tr>";

              $("#mytable").append(tr+td1+td2+td3+td4+td5);

            }
            $("#mytable").attr("border","1");


          },
          error: function(jqXHR, textStatus, errorThrown) {

          }
      });
      //updates lists of calendars
      $("#calViewSelect").empty();
      $("#eventViewSelect").empty();
      $("#calView2").html("");
      $.ajax({
          type: 'get',            //Request type
                 //Data type - we will use JSON for almost everything
          url: '/allFileNames',   //The server endpoint we are connecting to
          success: function (data) {
            let select = document.getElementById('calViewSelect');
            let select2 = document.getElementById('eventViewSelect');

            for(let i=0;i<data.length;i++)
            {
              var opt=document.createElement('option');
              opt.value=data[i];
              opt.innerHTML=data[i];
              select.add(opt);
            }
            for(let i=0;i<data.length;i++)
            {
              var opt=document.createElement('option');
              opt.value=data[i];
              opt.innerHTML=data[i];
              select2.add(opt);
            }

          },
          error: function(jqXHR, textStatus, errorThrown) {

          }
      });

      $.ajax({
          type: 'get',            //Request type
          dataType: 'text',       //Data type - we will use JSON for almost everything
          url: '/someendpoint',   //The server endpoint we are connecting to
          success: function (data) {
            var e = document.getElementById('calViewSelect');
            var strUser = e.options[e.selectedIndex].value;

            $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything
                url: '/getEvents',
                data: {strUser},   //The server endpoint we are connecting to
                success: function (data) {
                  var tbl=$("<table/>").attr("id","mytable2");
                  $("#calView2").append(tbl);
                  var tr="<tr>";
                  var td1="<td>Event No.</td>";
                  var td2="<td>Start Date</td>";
                  var td3="<td>Start Time</td>";
                  var td4="<td>Summary</td>";
                  var td5="<td>Num Props</td>";
                  var td6="<td>Num Alarms</td></tr>";
                  $("#mytable2").append(tr+td1+td2+td3+td4+td5+td6);

                  for(let i=0;i<Object.keys(data).length;i++)
                  {
                    let date=data[i]["startDT"].date.toString().substring(0,4)+"/"+data[i]["startDT"].date.toString().substring(4,6)+"/"+data[i]["startDT"].date.toString().substring(6,8);
                    let time=data[i]["startDT"].time.toString().substring(0,2)+":"+data[i]["startDT"].time.toString().substring(2,4)+":"+data[i]["startDT"].time.toString().substring(4,6);
                    if(data[i]["startDT"].UTC==true)
                    {
                      time.append(" (UTC)");
                    }
                    var tr="<tr>";
                    var td1="<td>"+(i+1)+"</td>";
                    var td2="<td>"+date+"</td>";
                    var td3="<td>"+time+"</td>";
                    var td4="<td>"+data[i]["summary"]+"</td>";
                    var td5="<td>"+data[i]["numProps"]+"</td>";
                    var td6="<td>"+data[i]["numAlarms"]+"</td></tr>";

                    $("#mytable2").append(tr+td1+td2+td3+td4+td5+td6);
                  }
                  $("#mytable2").attr("border","1");

                  let select = document.getElementById('eventChoice');

                  for(let i=0;i<Object.keys(data).length;i++)
                  {
                    var opt=document.createElement('option');
                    opt.value=data[i];
                    opt.innerHTML=i+1;
                    select.add(opt);
                  }
                },
                error: function(jqXHR, textStatus, errorThrown) {
                    // Non-200 return, do something with error
                    alert(errorThrown);
                }
            });


          },
          error: function(jqXHR, textStatus, errorThrown) {
              // Non-200 return, do something with error
              alert(errorThrown);
          }
      });
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the
        //$.ajax({});
    });

    /*$('#uploadFile').submit(function(e){
      $("#calViewSelect").empty();
      $("#eventViewSelect").empty();
      $("#calView2").html("");
      $("#fileView").html("");
      //Creates table of calendars
      $.ajax({
          type: 'get',            //Request type
                 //Data type - we will use JSON for almost everything
          url: '/pageLoadFiles',   //The server endpoint we are connecting to
          success: function (data) {

            var tbl=$("<table/>").attr("id","mytable");

            $("#fileView").append(tbl);
            var tr="<tr>";
            var td1="<td>File Name</td>";
            var td2="<td>Version</td>";
            var td3="<td>Number of Properties</td>";
            var td4="<td>Number of Events</td>";
            var td5="<td>Product ID</td></tr>";

            $("#mytable").append(tr+td1+td2+td3+td4+td5);

            for(var i=0;i<data.length;i++)
            {
              var tr="<tr>";
              var td1="<td>"+data[i]["filename"]+"</td>";
              var td2="<td>"+data[i]["version"]+"</td>";
              var td3="<td>"+data[i]["numProps"]+"</td>";
              var td4="<td>"+data[i]["numEvents"]+"</td>";
              var td5="<td>"+data[i]["prodID"]+"</td></tr>";

              $("#mytable").append(tr+td1+td2+td3+td4+td5);

            }
            $("#mytable").attr("border","1");


          },
          error: function(jqXHR, textStatus, errorThrown) {
          }
      });
      //Creates lists of calendars
      $.ajax({
          type: 'get',            //Request type
                 //Data type - we will use JSON for almost everything
          url: '/allFileNames',   //The server endpoint we are connecting to
          success: function (data) {
            let select = document.getElementById('calViewSelect');
            let select2 = document.getElementById('eventViewSelect');

            for(let i=0;i<data.length;i++)
            {
              var opt=document.createElement('option');
              opt.value=data[i];
              opt.innerHTML=data[i];
              select.add(opt);
            }
            for(let i=0;i<data.length;i++)
            {
              var opt=document.createElement('option');
              opt.value=data[i];
              opt.innerHTML=data[i];
              select2.add(opt);
            }

          },
          error: function(jqXHR, textStatus, errorThrown) {

          }
      });
      //populates event table with first calendar
      $.ajax({
          type: 'get',            //Request type
          dataType: 'text',       //Data type - we will use JSON for almost everything
          url: '/someendpoint',   //The server endpoint we are connecting to
          success: function (data) {
            var e = document.getElementById('calViewSelect');
            var strUser = e.options[e.selectedIndex].value;

            $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything
                url: '/getEvents',
                data: {strUser},   //The server endpoint we are connecting to
                success: function (data) {
                  var tbl=$("<table/>").attr("id","mytable2");

                  $("#calView2").append(tbl);
                  var tr="<tr>";
                  var td1="<td>Event No.</td>";
                  var td2="<td>Start Date</td>";
                  var td3="<td>Start Time</td>";
                  var td4="<td>Summary</td>";
                  var td5="<td>Num Props</td>";
                  var td6="<td>Num Alarms</td></tr>";
                  $("#mytable2").append(tr+td1+td2+td3+td4+td5+td6);

                  for(let i=0;i<Object.keys(data).length;i++)
                  {
                    let date=data[i]["startDT"].date.toString().substring(0,4)+"/"+data[i]["startDT"].date.toString().substring(4,6)+"/"+data[i]["startDT"].date.toString().substring(6,8);
                    let time=data[i]["startDT"].time.toString().substring(0,2)+":"+data[i]["startDT"].time.toString().substring(2,4)+":"+data[i]["startDT"].time.toString().substring(4,6);
                    if(data[i]["startDT"].UTC==true)
                    {
                      time.append(" (UTC)");
                    }
                    var tr="<tr>";
                    var td1="<td>"+(i+1)+"</td>";
                    var td2="<td>"+date+"</td>";
                    var td3="<td>"+time+"</td>";
                    var td4="<td>"+data[i]["summary"]+"</td>";
                    var td5="<td>"+data[i]["numProps"]+"</td>";
                    var td6="<td>"+data[i]["numAlarms"]+"</td></tr>";

                    $("#mytable2").append(tr+td1+td2+td3+td4+td5+td6);
                  }
                  $("#mytable2").attr("border","1");

                  let select = document.getElementById('eventChoice');

                  for(let i=0;i<Object.keys(data).length;i++)
                  {
                    var opt=document.createElement('option');
                    opt.value=data[i];
                    opt.innerHTML=i+1;
                    select.add(opt);
                  }
                },
                error: function(jqXHR, textStatus, errorThrown) {
                    // Non-200 return, do something with error
                    alert(errorThrown);
                }
            });


          },
          error: function(jqXHR, textStatus, errorThrown) {
              // Non-200 return, do something with error
              alert(errorThrown);
          }
      });

      $('#statView').append("Uploaded new file.<br>");
      e.preventDefault();

    });*/
    $('#clearData').click(function(e){

      $.ajax({
          type: 'get',      //Data type - we will use JSON for almost everything
          url: '/clearData',   //The server endpoint we are connecting to
          success: function (data) {

          },
          error: function(jqXHR, textStatus, errorThrown) {

          }
      });

        $('#statView').append("Clear data button pressed.<br>");
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the
        //$.ajax({});
    });
    $('#storeFiles').click(function(e){

      $.ajax({
          type: 'get',      //Data type - we will use JSON for almost everything
          url: '/storeFiles',   //The server endpoint we are connecting to
          success: function (data) {

          },
          error: function(jqXHR, textStatus, errorThrown) {

          }
      });

        $('#statView').append("Store all files button pressed.<br>");
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the
        //$.ajax({});
    });
    $('#dbStatus').click(function(e){

      $.ajax({
          type: 'get',      //Data type - we will use JSON for almost everything
          url: '/dbStatus',   //The server endpoint we are connecting to
          success: function (data) {

            $('#statView').append(data+"<br>");
          },
          error: function(jqXHR, textStatus, errorThrown) {

          }
      });

      e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the
        //$.ajax({});
    });
    $('#dbGo').click(function(e){
      let element = document.getElementById('dbSelect');
      let strUser2 = element.options[element.selectedIndex].value;
      if(strUser2=="displayDate")
      {
        displayDate();
      }
      else if(strUser2=="displayEvents")
      {
        let element2 = document.getElementById('dbEventSelect');
        let strUser3 = element2.options[element2.selectedIndex].value;
        displayEvents(strUser3);
      }
      else if(strUser2=="conflictEvents")
      {
        conflictingEvents();
      }
      else if(strUser2=="versionNumber")
      {
        let element2=$('#versionNumberQuery').val();
        versionNumber(element2);
      }
      else if(strUser2=="conflictAlarms")
      {
        alarmConflicts();
      }
      else if(strUser2=="eventsWithAlarms")
      {
        alarmsIn();
      }
      e.preventDefault();
    });
    $('#LOGIN').submit(function(e){

      let hi= {username:$('#username').val(),password:fileName=$('#password').val(),database:$('#database').val()};
      $.ajax({
          type: 'get',      //Data type - we will use JSON for almost everything
          url: '/login',
          data: {hi},   //The server endpoint we are connecting to
          success: function (data) {
            if(data=="FAILED")
            {
              $('#statView').append("Log in failed, Incorrect password/username.<br>");
            }
            else
            {
              $('#loggingin').hide();
              $('#databaseStuff').show();
              $('#statView').append("Logged into mySQL server<br>");
            }

          //console.log(data);

          },
          error: function(jqXHR, textStatus, errorThrown) {

          }
      });
      e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the
        //$.ajax({});
    });


});

function selectMenu()
{
  var e = document.getElementById('calViewSelect');
  var strUser = e.options[e.selectedIndex].value;
  $("#calView2").html("");
  $.ajax({
      type: 'get',            //Request type
      dataType: 'json',       //Data type - we will use JSON for almost everything
      url: '/getEvents',
      data: {strUser},   //The server endpoint we are connecting to
      success: function (data) {
        var tbl=$("<table/>").attr("id","mytable2");

        $("#calView2").append(tbl);
        var tr="<tr>";
        var td1="<td>Event No.</td>";
        var td2="<td>Start Date</td>";
        var td3="<td>Start Time</td>";
        var td4="<td>Summary</td>";
        var td5="<td>Num Props</td>";
        var td6="<td>num Alarms</td></tr>";
        $("#mytable2").append(tr+td1+td2+td3+td4+td5+td6);

        for(let i=0;i<Object.keys(data).length;i++)
        {
          let date=data[i]["startDT"].date.toString().substring(0,4)+"/"+data[i]["startDT"].date.toString().substring(4,6)+"/"+data[i]["startDT"].date.toString().substring(6,8);
          let time=data[i]["startDT"].time.toString().substring(0,2)+":"+data[i]["startDT"].time.toString().substring(2,4)+":"+data[i]["startDT"].time.toString().substring(4,6);
          if(data[i]["startDT"].UTC==true)
          {
            time.append(" (UTC)");
          }
          var tr="<tr>";
          var td1="<td>"+(i+1)+"</td>";
          var td2="<td>"+date+"</td>";
          var td3="<td>"+time+"</td>";
          var td4="<td>"+data[i]["summary"]+"</td>";
          var td5="<td>"+data[i]["numProps"]+"</td>";
          var td6="<td>"+data[i]["numAlarms"]+"</td></tr>";

          $("#mytable2").append(tr+td1+td2+td3+td4+td5+td6);
        }
        $("#mytable2").attr("border","1");


        $('#eventChoice').empty();
        let select = document.getElementById('eventChoice');

        for(let i=0;i<Object.keys(data).length;i++)
        {
          var opt=document.createElement('option');
          opt.value=data[i];
          opt.innerHTML=i+1;
          select.add(opt);
        }
      },
      error: function(jqXHR, textStatus, errorThrown) {
          // Non-200 return, do something with error
          alert(errorThrown);
      }
  });

}

function displayDate()
{
  $.ajax({
      type: 'get',      //Data type - we will use JSON for almost everything
      url: '/displayDate',   //The server endpoint we are connecting to
      success: function (data) {
        let hi="";
        for(let i=0;i<data.length;i++)
        {
          hi+="EVENT "+(i+1)+"\n"
          hi+="SUMMARY: "+data[i]["summary"]+"\n";
          hi+="START TIME: "+data[i]["start_time"]+"\n";
          hi+="\n\n";
        }
        alert(hi);
        $('#statView').append("Displayed all events sorted by date<br>");
      },
      error: function(jqXHR, textStatus, errorThrown) {

      }
  });

  //e.preventDefault();
}

function updateDBThing()
{
  let element = document.getElementById('dbSelect');
  let strUser2 = element.options[element.selectedIndex].value;
  if(strUser2=="displayEvents")
  {
    $('#displayEventThings').html("<select id = \"dbEventSelect\"></select>");
    $.ajax({
        type: 'get',            //Request type
               //Data type - we will use JSON for almost everything
        url: '/allFileNames',   //The server endpoint we are connecting to
        success: function (data) {
          let select = document.getElementById('dbEventSelect');

          for(let i=0;i<data.length;i++)
          {
            var opt=document.createElement('option');
            opt.value=data[i];
            opt.innerHTML=data[i];
            select.add(opt);
          }


        },
        error: function(jqXHR, textStatus, errorThrown) {

        }
    });
  }
  else if(strUser2=="displayDate")
  {
    $('#displayEventThings').html("");
  }
  else if(strUser2=="conflictEvents")
  {
    $('#displayEventThings').html("");
  }
  else if(strUser2=="versionNumber")
  {
    $('#displayEventThings').html("<input value=\"1\" id=\"versionNumberQuery\" type=NUMBER name=\"versionNumberQuery\"><br>");
  }
  else if(strUser2=="conflictAlarms")
  {
    $('#displayEventThings').html("");
  }
}

function displayEvents(filename)
{
  $.ajax({
      type: 'get',      //Data type - we will use JSON for almost everything
      url: '/displayEvents',
      data: {filename},   //The server endpoint we are connecting to
      success: function (data) {
        let hi="";
        for(let i=0;i<data.length;i++)
        {
          hi+="EVENT "+(i+1)+"\n"
          hi+="SUMMARY: "+data[i]["summary"]+"\n";
          hi+="START TIME: "+data[i]["start_time"]+"\n";
          hi+="\n\n";
        }
        alert(hi);
        $('#statView').append("Displayed all events in file<br>");
      },
      error: function(jqXHR, textStatus, errorThrown) {

      }
  });

  //e.preventDefault();
}

function conflictingEvents()
{
  $.ajax({
      type: 'get',      //Data type - we will use JSON for almost everything
      url: '/conflicts',   //The server endpoint we are connecting to
      success: function (data) {
        let hi="";
        for(let i=0;i<data.length;i++)
        {
          hi+="EVENT "+(i+1)+"\n"
          hi+="SUMMARY: "+data[i]["summary"]+"\n";
          hi+="START TIME: "+data[i]["start_time"]+"\n";
          hi+="\n\n";
        }
        alert(hi);
        $('#statView').append("Displayed all events in file<br>");
      },
      error: function(jqXHR, textStatus, errorThrown) {

      }
  });
}

function versionNumber(number)
{
  $.ajax({
      type: 'get',      //Data type - we will use JSON for almost everything
      url: '/versionNumber',
      data: {number},   //The server endpoint we are connecting to
      success: function (data) {
        let hi="";
        for(let i=0;i<data.length;i++)
        {
          hi+="FILE "+(i+1)+"\n"
          hi+="FILENAME: "+data[i]["file_name"]+"\n";
          hi+="PRODUCT ID: "+data[i]["prod_id"]+"\n";
          hi+="\n\n";
        }
        alert(hi);
        $('#statView').append("Displayed all events in file<br>");
      },
      error: function(jqXHR, textStatus, errorThrown) {

      }
  });
}

function alarmConflicts()
{
  $.ajax({
      type: 'get',      //Data type - we will use JSON for almost everything
      url: '/alarmConflicts',   //The server endpoint we are connecting to
      success: function (data) {
        let hi="";
        for(let i=0;i<data.length;i++)
        {
          hi+="ALARM "+(i+1)+"\n"
          hi+="TRIGGER: "+data[i]["trigger"]+"\n";
          hi+="ACTION: "+data[i]["action"]+"\n";
          hi+="\n\n";
        }
        if(data.length!=0)
        {
          alert(hi);
        }

        $('#statView').append("Displayed all conflicting alarms in file<br>");
      },
      error: function(jqXHR, textStatus, errorThrown) {

      }
  });
}

function alarmsIn()
{
  $.ajax({
      type: 'get',      //Data type - we will use JSON for almost everything
      url: '/alarmsIn',   //The server endpoint we are connecting to
      success: function (data) {
        let hi="";
        for(let i=0;i<data.length;i++)
        {
          hi+="EVENT "+(i+1)+"\n"
          hi+="SUMMARY: "+data[i]["summary"]+"\n";
          hi+="START TIME: "+data[i]["start_time"]+"\n";
          hi+="ALARM\n";
          hi+="TRIGGER: "+data[i]["trigger"]+"\n";
          hi+="ACTION: "+data[i]["action"]+"\n";
          hi+="\n\n";
        }
        if(data.length!=0)
        {
          alert(hi);
        }

        $('#statView').append("Displayed all events with alarms<br>");
      },
      error: function(jqXHR, textStatus, errorThrown) {

      }
  });
}






//dsds
