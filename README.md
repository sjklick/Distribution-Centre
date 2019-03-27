Automated-Order-Picker
======================

Summary
-------

This is a simplified simulation of an Amazon-like service. There are two main aspects to this application.
First, a page where users can view products, fill a cart, and place fictitious orders. Second, a page
which shows a simulated warehouse in action. This project is being developed for learning purposes.

Features To Be Implemented
--------------------------
- A MySQL database containing fictitious products, quantity available, etc...
- A page where users can order products
  - Good opportunity to experiment with React or Angular framework(s)
  - A "Featured Products" section
    - Maybe include top-sellers (maximize sales) and some poor-sellers (to move inventory)
  - Select products by category
  - A viewable cart
  - Form to collect user data (name, email)
  - Order summary (list of items, name, email, order number)
  - Order should be sent to simulated fulfillment centre
  - If there are too many orders queued, inform the user that there is "higher than usual volume"
    and ask them to try again later
- A page where users can view the warehouse as it operates
  - Users can watch the automated picker robots navigate the warehouse and collect items for shipping
  - Users can view the queue of order numbers (including current order)
  - Allow users to select individual order pickers
    - View path, destination, item, etc...
- A back-end
  - PHP to manipulate MySQL database and deliver relevant content to front-end pages
  - Maintain user sessions
  - Allow warehouse to request items from vendors. These must be ordered in groups (i.e. don't request
    a shipment for a single item), and only once the order pickers have cleared the receiving area
  - C++ programs for some of the heavy lifting (eg. A* Pathfinding routine)
  - User should receive email confirmation stating that the order has been received
  - User should receive email confirmation stating that the order has shipped
- At the completion of the project, a live demo will be hosted and linked to in the summary

Contact
-------

*Author*: Steven Joshua Klickermann\
*Email*: sjklickermann@gmail.com\
*GitHub*: https://github.com/sjklick

License
-------

This project is licensed under the terms of the GNU General Public
License v3.0 (see LICENSE.md).
