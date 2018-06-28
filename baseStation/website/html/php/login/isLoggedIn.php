<?php

session_start();

if(isset($_SESSION["u_id"])) {
    echo "true";
    exit();
} else {
    echo "false";
    exit();
}