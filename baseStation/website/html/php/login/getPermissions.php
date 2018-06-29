<?php

session_start();

if(isset($_SESSION['u_id'])) {
    $priv = new class{};

    $priv->add_usr = $_SESSION['u_priv_add_usr'];
    $priv->mod_priv = $_SESSION['u_priv_mod_priv'];
    $priv->reboot = $_SESSION['u_priv_reboot'];
    $priv->mod_sett = $_SESSION['u_priv_mod_sett'];

    echo json_encode($priv);
}