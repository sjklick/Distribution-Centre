Distribution-Centre Web API User Guide
======================================

Summary of API Requests
-----------------------

* \<host-domain\>/api/bin_item_counts/read.php
* \<host-domain\>/api/bin_positions/read.php
* \<host-domain\>/api/bin/read.php
* \<host-domain\>/api/categories/read.php
* \<host-domain\>/api/order/place.php
* \<host-domain\>/api/orders/read.php
* \<host-domain\>/api/pickers/read.php
* \<host-domain\>/api/products/read.php
* \<host-domain\>/api/products/read.php?category=furniture

API Request Details
-------------------

**\<host-domain\>/api/bin_item_counts/read.php**

Returns the total count of items (0-12) in each bin. Use request method GET.

	{
		"stock": [
			<int>,

  		],
  		"receiving": <int>,
		"shipping": <int>
	}

---

**\<host-domain\>/api/bin_positions/read.php**

Returns all stock bin positions, ordered by bin ID. Use request method GET.

	[
		{
			"row": <int>,
			"column": <int>,
			"facing": <string>
		},
		...
	]

---

**\<host-domain\>/api/bin/read.php**

---

**\<host-domain\>/api/categories/read.php**

---

**\<host-domain\>/api/order/place.php**

---

**\<host-domain\>/api/orders/read.php**

Returns the order IDs of all placed orders, starting with the order currently being processed. Use request method GET.

	[
		<int>,
		...
	]

---

**\<host-domain\>/api/pickers/read.php**

Returns information regarding all automated order pickers including position, state, and whether or not an item is being transported. Use request method GET.

	[  
		{  
			"row": <int>,  
			"column": <int>,  
			"facing": <string>,  
			"state": <string>,  
			"item": <bool>  
		},  
		...  
	]

---

**\<host-domain\>/api/products/read.php**

---

**\<host-domain\>/api/products/read.php?category=furniture**