OrderPicker::OrderPicker(Position home) {
    this->home = home;
    current = home;
    path.clear();
    state = idle;
}

Position OrderPicker::getPosition() {
    return current;
}

std::vector<Position> OrderPicker::getPath() {
}

State OrderPicker::getState() {
    return state;
}

OrderPicker::~OrderPicker() {
    // nothing for now
}
