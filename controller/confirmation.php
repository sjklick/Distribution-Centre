<?php

if (!isset($argc)) die();
if ($argc != 4) die();
$to = $argv[3];
$subject = "Confirmation: Fictitious Order Shipped";
$message = "Hi ".$argv[2].",\n\n";
$message .= "This email is to confirm that your fictitious order (ID: ";
$message .= $argv[1];
$message .= ") has shipped, and this completes your order. ";
$message .= "Thank you for choosing our fictitous service.";
$additional_headers = "From: sjk.dev.test@gmail.com";
mail($to, $subject, $message, $additional_headers);

?>