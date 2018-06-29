<?php

session_start();

if(isset($_SESSION["u_id"])) {
    echo $_SESSION["u_name"];
    exit();
} else {
    echo "false";
    exit();
}