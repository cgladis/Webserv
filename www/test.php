<?php
session_start();
?>

<html>
<head>
    <link rel="stylesheet" href="../css/cgi_main.css">
    <link href="https://fonts.googleapis.com/css2?family=Raleway:wght@100;300;400;500;600;700;800&display=swap" rel="stylesheet">
</head>

<body>
<div class="content">
    <h1> You visited this page <span>

    <?php
    if (!isset($_SESSION['count']))
    {
      $_SESSION['count'] = 1;
    }
    else
    {
      ++$_SESSION['count'];
    }


    echo $_SESSION['count'];

    ?>

    <span> times.<h1>
</div>
</body>
</html>