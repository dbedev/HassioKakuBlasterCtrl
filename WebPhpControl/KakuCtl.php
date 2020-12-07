<?php
echo "Kaku Send Control<br>";

if(isset($_GET['KakuAdr'])){
    $myKakuAdr = $_GET['KakuAdr'];
	if(isset($_GET['KakuCmd'])){
		$myKakuCmd = $_GET['KakuCmd'];
		if(isset($_GET['KakuVal'])){
		   $myKakuVal = $_GET['KakuVal'];
		   echo ("Calling Kaku <br>");
		   exec("KakuSend /dev/ttyKaku \"Kaku $myKakuAdr $myKakuCmd $myKakuVal \" 400 ",$CallResponse,$funcRes );
                   foreach($CallResponse as $retLine){
                      echo "$retLine <br>";
                   }
                   echo "Result: $funcRes <br>";
	        }
	}
}
if(isset($_GET['KakuStr'])){
    $myKakuFullCmd = $_GET['KakuStr'];
    echo ("Calling Kaku <br> $myKakuFullCmd <br>");
    exec("KakuSend /dev/ttyKaku $myKakuFullCmd 400 ",$CallResponse,$funcRes );
    foreach($CallResponse as $retLine){
      echo "$retLine <br>";
    }
    echo "Result: $funcRes <br>";
}

?>
