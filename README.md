Automated-Order-Picker
======================

<p>
<img src="https://www.w3.org/html/logo/downloads/HTML5_Logo.svg" width="100" height="100" />
<img src="https://upload.wikimedia.org/wikipedia/commons/d/d5/CSS3_logo_and_wordmark.svg" width="100" height="100" />
<img src="https://upload.wikimedia.org/wikipedia/commons/9/99/Unofficial_JavaScript_logo_2.svg" width="100" height="100" />
<img src="https://www.php.net/images/logos/new-php-logo.svg" width="100" height=100" /> <!-- Courtesy of Colin Viebrock, Creative Commons Attribution-Share Alike 4.0 International: https://creativecommons.org/licenses/by-sa/4.0/ -->
<img src="https://upload.wikimedia.org/wikipedia/commons/1/18/ISO_C%2B%2B_Logo.svg" width="100" height="100" /> <!-- Courtesy of Jeremy Kratz, The Standard C++ Foundation, https://isocpp.org/home/terms-of-use -->
<img src="https://upload.wikimedia.org/wikipedia/commons/6/68/Mariadb-seal-browntext.svg" width="300" height="100" /> <!-- Courtesy of Mike Zinner of [mariadb.org],  Creative Commons Attribution-Share Alike 3.0 Unported: https://creativecommons.org/licenses/by-sa/3.0/deed.en -->
</p>

Summary
-------

This is a simplified simulation of an automated distribution centre. An SQL database (MariaDB) maintains a list of products, locations in the warehouse, order-queue, and products in shipping/stock/receiving. A C++ program simulates a series of automated order pickers that handle incoming stock in receiving, and move items from stock to shipping to satisfy the current order. A web interface is provided for viewing the warehouse. An API will be provided to allow anyone to create a front-end to interact with the warehouse and place fictitious orders.

Todo
----
- Expand API to allow pages of products to be selected
- Make API use pretty URLs
- Finalize available products and descriptions
- Fix bug in order placement so orders cannot be placed when inventory is lacking
- Document the web API
- Host live demo

Contact
-------

*Author:* Steven Joshua Klickermann\
*Email:* sjklickermann@gmail.com\
*GitHub:* https://github.com/sjklick

License
-------

This project is licensed under the terms of the GNU General Public
License v3.0 (see LICENSE.md).
