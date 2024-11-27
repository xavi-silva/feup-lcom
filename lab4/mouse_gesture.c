#include "mouse_gesture.h"



GestureState currentState = START;
MouseEvents mouseEvents;
uint8_t current_x_len = 0;
extern struct packet packetStruct; 

int update_mouse_events(uint8_t tolerance){
    mouseEvents.LeftButtonPressed = mouseEvents.RightButtonPressed = mouseEvents.NoButtonPressed = mouseEvents.Ascend = mouseEvents.Descend = mouseEvents.Tolerance = false;
    if(packetStruct.lb && !(packetStruct.rb) && !(packetStruct.mb)){
        mouseEvents.LeftButtonPressed = true;
    }
    if(packetStruct.rb && !(packetStruct.lb) && !(packetStruct.mb)){
        mouseEvents.RightButtonPressed = true;
    }
    if(!(packetStruct.lb || packetStruct.rb || packetStruct.mb)){
        mouseEvents.NoButtonPressed = true;
    }
    if(packetStruct.delta_x > 0 && packetStruct.delta_y > 0){
        mouseEvents.Ascend = true;
    }
    if(packetStruct.delta_x > 0 && packetStruct.delta_y < 0){
        mouseEvents.Descend = true;
    }
    if(abs(packetStruct.delta_x) <= tolerance && abs(packetStruct.delta_y) <= tolerance){
        mouseEvents.Tolerance = true;
    }
    return 0;
}
int update_gesture_state(uint8_t x_len, uint8_t tolerance){
    update_mouse_events(tolerance);
    switch(currentState){
        case START:{
            if(mouseEvents.LeftButtonPressed){
                current_x_len = 0;
                currentState = DIAGONAL1;
            }
            else{
                //Stay in START
            }
            break;
        }
        case DIAGONAL1:{
            current_x_len = 0 > packetStruct.delta_x ? current_x_len : current_x_len + packetStruct.delta_x;
            if(mouseEvents.Ascend || mouseEvents.Tolerance){
                if((current_x_len >= x_len) && mouseEvents.NoButtonPressed){
                    currentState = VERTEX;
                }
                else if(!mouseEvents.LeftButtonPressed){
                    currentState = START;
                }
            }
            else currentState = START;
            break;
        }
        case VERTEX:{
            if(mouseEvents.RightButtonPressed){
                current_x_len = 0;
                currentState = DIAGONAL2;
            }
            else if(mouseEvents.NoButtonPressed && mouseEvents.Tolerance){
                //Stay in VERTEX
            }
            else{
                currentState = START;
            }
            break;
        }
        case DIAGONAL2:{
            current_x_len = 0 > packetStruct.delta_x ? current_x_len : current_x_len + packetStruct.delta_x;
            if(mouseEvents.Descend || mouseEvents.Tolerance){
                if(current_x_len >= x_len && mouseEvents.NoButtonPressed){
                    currentState = END;
                }
                else if(!mouseEvents.RightButtonPressed){
                    currentState = START;
                }
            }
            else currentState = START;
            break;
        }
        case END:{
            break;
        }
        default:
            break;
    }
    return 0;
}
