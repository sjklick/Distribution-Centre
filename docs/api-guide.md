Distribution-Centre Web API User Guide
======================================

Summary of API Requests
-----------------------

* *\<host-domain\>*/api/bin_item_counts/read.php
* *\<host-domain\>*/api/bin_positions/read.php
* *\<host-domain\>*/api/bin/read.php
* *\<host-domain\>*/api/categories/read.php
* *\<host-domain\>*/api/order/place.php
* *\<host-domain\>*/api/orders/read.php
* *\<host-domain\>*/api/pickers/read.php
* *\<host-domain\>*/api/products/read.php
* *\<host-domain\>*/api/products/read.php?category=furniture

API Request Details
-------------------

*\<host-domain\>*/api/pickers/read.php

Description: Returns information regarding all automated order pickers including position, state, and whether or not an item is being transported.

Method: GET

>   [  
>       {  
>           "row": *\<int\>*,  
>           "column": *\<int\>*,  
>           "facing": *\<string\>*,  
>           "state": *\<string\>*,  
>           "item": *\<bool\>*  
>       },  
>       ...  
>   ]