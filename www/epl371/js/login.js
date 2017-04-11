function unique_requestid() {
    var timestamp = Math.random() * Number(new Date()).toString();
    var random = Math.random() * (Math.random() * 100000 * Math.random() );
    var unique = new String();
    unique = timestamp + random;
    return unique;
}
var GroupID = 1; // 1 for EPL034
var PassTypeID = 0;
var NoOfAttempts = 0;
var SecondsElapsed = 0;
var SelectedImages = "";
var percentage = 0;
// Javascript to compute elapsed time between "Start" and "Finish" button clicks
function timestamp_class(this_current_time, this_start_time, this_end_time, this_time_difference) {
    this.this_current_time = this_current_time;
    this.this_start_time = this_start_time;
    this.this_end_time = this_end_time;
    this.this_time_difference = this_time_difference;
    this.GetCurrentTime = GetCurrentTime;
    this.StartTiming = StartTiming;
    this.EndTiming = EndTiming;
}

//Get current time from date timestamp
function GetCurrentTime() {
    var my_current_timestamp;
    my_current_timestamp = new Date();		//stamp current date & time
    return my_current_timestamp.getTime();
}

//Stamp current time as start time and reset display textbox
function StartTiming() {
    this.this_start_time = GetCurrentTime();	//stamp current time
}

//Stamp current time as stop time, compute elapsed time difference and display in textbox
function EndTiming() {
    this.this_end_time = GetCurrentTime();		//stamp current time
    this.this_time_difference = (this.this_end_time - this.this_start_time) / 1000;	//compute elapsed time
    return this.this_time_difference;	//set elapsed time in display box
}

var time_object = new timestamp_class(0, 0, 0, 0);	//create new time object and initialize it

jQuery(document).ready(function ($) {
    time_object.StartTiming();

    $("#btnStep1").click(function () {

    });

    $("#btnLogin").click(function () {
        if (!$("#divPassText").is(":visible") && !$("#divPassFace").is(":visible")) {
            $(".status").html('<img src="images/wait.gif" align="absmiddle">&nbsp;Processing information...');
            var UserID = $("#txtUserID").val();
            if (UserID == "") {
                $(".status").html("<img src='images/delete_small.png' align='absmiddle'>&nbsp;<b>Please fill in your password</b>");
            }
            else {
                $.ajax({
                    type: "POST",
                    url: "phpScripts/getPassTypeID.php",
                    data: "UserID=" + UserID + "&uniqueID=" + unique_requestid(),
                    success: function (msg) {
                        $(".status").one("ajaxComplete", function (event, request, settings) {
                            if (msg.substring(0, 5) != "Error") {
                                $(this).html("");
                                var splittedArray = msg.split("|");
                                PassTypeID = splittedArray[0];
                                LoginsBeforeChange = splittedArray[1];
                                IsPassTypeChanged = splittedArray[2];
                                if (IsPassTypeChanged == 1 || LoginsBeforeChange > 0) {
                                    if (IsPassTypeChanged == 0 && LoginsBeforeChange > 0)
                                        $("#notification").show();
                                    if (PassTypeID == 1)
                                        $("#divPassText").show("slow");
                                    else if (PassTypeID == 2)
                                        $("#divPassFace").show("slow");
                                }
                                else if (IsPassTypeChanged == 0 && LoginsBeforeChange <= 0) {
                                    if (PassTypeID == 1)
                                        location.href = "updatePassword.php?PassTypeID=2&UserID=" + UserID;
                                    else
                                        location.href = "updatePassword.php?PassTypeID=1&UserID=" + UserID;
                                }
                                //$("#btnStep1").hide("slow");
                                //$("#btnLogin").show("slow");
                            }
                            else {
                                $(this).html("<img src='images/delete_small.png' align='absmiddle'>&nbsp;<b>" + msg + "</b>");
                            }
                        });
                    }
                });
            }
        }
        else {
            $(".status").html('<img src="images/wait.gif" align="absmiddle">&nbsp;Processing information...');
            var CountSelectedImages = 0;
            var UserID = $("#txtUserID").val();
            var Password = $("#txtPassword").val();
            SecondsElapsed = time_object.EndTiming();
            NoOfAttempts++;
            if (PassTypeID == 2) {
                SelectedImages = "";
                $('#animal_fish tr td img').each(function () {
                    ImageID = $(this).attr("id").replace("img", "");
                    if ($(this).parent().hasClass("selected")) {
                        CountSelectedImages++;
                        SelectedImages += ImageID + ",";
                    }
                });
                Password = SelectedImages;
            }
            if (UserID == "" || Password == "") {
                $(".status").html("<img src='images/delete_small.png' align='absmiddle'>&nbsp;<b>Please fill in all the required fields</b>");
            }
            else {
                $.ajax({
                    type: "POST",
                    url: "phpScripts/oLogin.php",
                    data: "UserID=" + UserID + "&GroupID=" + GroupID + "&PassTypeID=" + PassTypeID + "&Password=" + Password + "&NoOfAttempts=" + NoOfAttempts + "&SecondsElapsed=" + SecondsElapsed + "&uniqueID=" + unique_requestid(),
                    success: function (msg) {
                        $(".status").one("ajaxComplete", function (event, request, settings) {
                            if (msg.substring(0, 5) != "Error") {
                                $(this).html("<img src='images/accepted.png' align='absmiddle'>&nbsp;<b>" + msg + "</b>");
                                location.reload()
                            }
                            else {
                                $(this).html("<img src='images/delete_small.png' align='absmiddle'>&nbsp;<b>" + msg + "</b>");
                            }
                        });
                    }
                });
            }
        }
    });

    $('.smallImage').click(function () {
        $(this).parent().toggleClass("selected");
    });
});

function goTo(url) {
    var a = document.createElement("a");
    if (a.click) {
        // HTML5 browsers and IE support click() on <a>, early FF does not.
        a.setAttribute("href", url);
        a.style.display = "none";
        document.body.appendChild(a);
        a.click();
    } else {
        // Early FF can, however, use this usual method
        // where IE cannot with secure links.
        window.location = url;
    }
}