<!DOCTYPE html>
<html lang="en">

<html>
    <head>
        <meta charset="utf-8">
        <meta http-equiv="X-UA-Compatible" content="IE=edge">
        <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
        <meta name="robots" content="noindex" />
        <title>Weather Station</title>
        
        <!-- Favicon -->
        <link rel="apple-touch-icon" sizes="57x57" href="/favicon/apple-icon-57x57.png">
        <link rel="apple-touch-icon" sizes="60x60" href="/favicon/apple-icon-60x60.png">
        <link rel="apple-touch-icon" sizes="72x72" href="/favicon/apple-icon-72x72.png">
        <link rel="apple-touch-icon" sizes="76x76" href="/favicon/apple-icon-76x76.png">
        <link rel="apple-touch-icon" sizes="114x114" href="/favicon/apple-icon-114x114.png">
        <link rel="apple-touch-icon" sizes="120x120" href="/favicon/apple-icon-120x120.png">
        <link rel="apple-touch-icon" sizes="144x144" href="/favicon/apple-icon-144x144.png">
        <link rel="apple-touch-icon" sizes="152x152" href="/favicon/apple-icon-152x152.png">
        <link rel="apple-touch-icon" sizes="180x180" href="/favicon/apple-icon-180x180.png">
        <link rel="icon" type="image/png" sizes="192x192"  href="/favicon/android-icon-192x192.png">
        <link rel="icon" type="image/png" sizes="32x32" href="/favicon/favicon-32x32.png">
        <link rel="icon" type="image/png" sizes="96x96" href="/favicon/favicon-96x96.png">
        <link rel="icon" type="image/png" sizes="16x16" href="/favicon/favicon-16x16.png">
        <link rel="manifest" href="/manifest.json">
        <meta name="msapplication-TileColor" content="#ffffff">
        <meta name="msapplication-TileImage" content="/favicon/ms-icon-144x144.png">
        <meta name="theme-color" content="#ffffff">
        
        
        <!-- Bootstrap core CSS-->
        <link href="/startbootstrap-sb-admin/vendor/bootstrap/css/bootstrap.min.css" rel="stylesheet">
        <!-- Custom fonts for this template-->
        <link href="/startbootstrap-sb-admin/vendor/font-awesome/css/font-awesome.min.css" rel="stylesheet" type="text/css">
        <!-- Page level plugin CSS-->
        <link href="/startbootstrap-sb-admin/vendor/datatables/dataTables.bootstrap4.css" rel="stylesheet">
        <!-- Custom styles for this template-->
        <link href="/startbootstrap-sb-admin/css/sb-admin.css" rel="stylesheet">
        
        <!-- Custom styles for this website -->
        <link href="/css/style.min.css" rel="stylesheet">
        
    </head>
    <body class="fixed-nav sticky-footer bg-dark" id="page-top">
        <!-- Navigation -->
        <nav class="navbar navbar-expand-lg navbar-dark bg-dark fixed-top" id="mainNav">
            
            <a class="navbar-brand" href="/">Weather Station</a>
            <button class="navbar-toggler navbar-toggler-right" type="button" data-toggle="collapse" data-target="#navbarResponsive" aria-controls="navbarResponsive" aria-expanded="false" aria-label="Toggle navigation">
                <span class="navbar-toggler-icon"></span>
            </button>
            
            <div class="collapse navbar-collapse" id="navbarResponsive">
                <ul class="navbar-nav navbar-sidenav" id="exampleAccordion">
                    
                    <li class="nav-item" data-toggle="tooltip" data-placement="right" title="Dashboard">
                        <a class="nav-link" href="/">
                            <i class="fa fa-fw fa-dashboard"></i>
                            <span class="nav-link-text">Dashboard</span>
                        </a>
                        <a class="nav-link" href="/weather">
                            <i class="fa fa-fw fa-area-chart"></i>
                            <span class="nav-link-text">Weather</span>
                        </a>
                        <a class="nav-link" href="/config">
                            <i class="fa fa-fw fa-cogs"></i>
                            <span class="nav-link-text">Settings</span>
                        </a>
                        <a class="nav-link" href="/start.html">
                            <i class="fa fa-fw fa-link"></i>
                            <span class="nav-link-text">Start Page</span>
                        </a>
                    </li>
                    
                </ul>
                <!-- Hide sidebar -->
                <ul class="navbar-nav sidenav-toggler">
                    <li class="nav-item">
                        <a class="nav-link text-center" id="sidenavToggler">
                        <i class="fa fa-fw fa-angle-left"></i>
                        </a>
                    </li>
                </ul>

                <!-- Top bar -->
                <ul class="navbar-nav ml-auto">
                    
                    <li class="nav-item user-loggedin-priv" id="user-change-priv">
                        <a class="nav-link" href="/user/privileges/">
                            <i class="fa fa-fw fa-server"></i>
                            Edit Permissions
                        </a>
                    </li>

                    <li class="nav-item user-loggedin-priv" id="user-add-user">
                        <a class="nav-link" href="/user/register/">
                            <i class="fa fa-fw fa-user-plus"></i>
                            Create User
                        </a>
                    </li>

                    <li class="nav-item user-loggedin">
                        <a class="nav-link" href="/user/settings/">
                            <i class="fa fa-fw fa-user-circle-o"></i>
                            User Settings
                        </a>
                    </li>

                    <li class="nav-item user-loggedin" id="user-logout">
                        <a class="nav-link" data-toggle="modal" data-target="#exampleModal">
                            <i class="fa fa-fw fa-sign-out"></i>
                            Logout
                        </a>
                    </li>

                    <li class="nav-item" id="user-login">
                        <a href="/user/login/" class="nav-link">
                            <i class="fa fa-fw fa-sign-in"></i>
                            Login
                        </a>
                    </li>
                </ul>
            </div>
        </nav>
        
        <div class="content-wrapper">
            
            <div class="container-fluid">
                <!-- Breadcrumb Page Title -->
                <ol class="breadcrumb">
                    <li class="breadcrumb-item">
                        <a href="index.html">DashBoard</a>
                    </li>
                    <li class="breadcrumb-item active">Settings</li>
                </ol>
                
                
                <!-- Body -->
                
                <!-- Status -->
                <div class="card mb-3">
                    <div class="card-header">Status</div>
                    <div class="card-body">
                        <div class="row">
                            <div class="col-sm-12">
                                <div class="alert alert-success" id="station-reporting"><strong>Successfully Reporting</strong></div>
                                <div class="alert alert-danger" id="station-not-reporting"><strong>Not Reporting</strong></div>
                            </div>
                        </div>
                        
                        <h5>Weather Station</h5>
                        <hr>
                        <div class="row">
                            <div class="col-lg-4 col-sm-6">
                                <p>Battery: <span id="station-battery"></span> V</p>
                                <p>Charging: <span id="station-isCharging"></span></p>
                                <p>Time Spent Charging: <span id="station-chargeTime"></span></p>
                            </div>
                            <div class="col-lg-4 col-sm-6">
                                <p>Uptime: <span id="station-uptime"></span></p>
                                <p>Lost Packets: <span id="station-lostPackets"></span></p>
                                <p>Number of resets: <span id="station-resets"></span> </p>
                            </div>
                        </div>
        
                        <h5>Base Station</h5>
                        <hr>
                        <div class="row">
                            <div class="col-sm-6">
                                <p>Uptime: <span id="base-uptime"></span></p>
                                <p>Resets: <span id="base-resets"></span></p>
                            </div>
                        </div>
                    </div>
                    <div class="card-footer small text-muted">Updated <span id="station-update-time">Never</span></div>
                </div>
                
                
            </div>
            
            
            <!-- Bootstrap core JavaScript-->
            <script src="/startbootstrap-sb-admin/vendor/jquery/jquery.min.js"></script>
            <script src="/js/includes/moment.min.js"></script>
            <script src="/startbootstrap-sb-admin/vendor/bootstrap/js/bootstrap.bundle.min.js"></script>
            <!-- Core plugin JavaScript-->
            <script src="/startbootstrap-sb-admin/vendor/jquery-easing/jquery.easing.min.js"></script>
            <!-- Page level plugin JavaScript-->
            <script src="/startbootstrap-sb-admin/vendor/chart.js/Chart.min.js"></script>
            <script src="/startbootstrap-sb-admin/vendor/datatables/jquery.dataTables.js"></script>
            <script src="/startbootstrap-sb-admin/vendor/datatables/dataTables.bootstrap4.js"></script>
            <!-- Custom scripts for all pages-->
            <script src="/startbootstrap-sb-admin/js/sb-admin.min.js"></script>
            <script src="/js/users/users.min.js"></script>
            
            <!-- Custom scripts for this page-->
            <script src="/js/status.min.js"></script>
            
        </div>
    </body>
</html>