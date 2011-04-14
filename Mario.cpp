/* 
 *  Mario.cpp
 *  Mario
 *
 *  Created by Andrew Peddicord on 2/1/11.
 *  Copyright 2011 Capital University. All rights reserved.
 *
 * filed edited by Drew and Nate
 */


#include "Mario.h"
#include "LListIterator.h"
#include "Level.h"
#include "Breakable.h"
#include "Nonbreakable.h"
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;


//------------------------------------------------------------
void Mario::draw()
{
    
    if (this->getYVelocity() != 0.0 && this->getXVelocity() >= 0.0) {
        texturePos = 3;
    }
    else if (this->getYVelocity() != 0.0 && this->getXVelocity() < 0.0){
        texturePos = 7;
    }
    else if (this->getXVelocity() > 0.0){
        if (texturePos == 1) {
            texturePos = 2;
        }
        else{
            texturePos = 1;
        }
    }
    else if (this->getXVelocity() < 0.0){
        if (texturePos == 5) {
            texturePos = 6;
        }
        else {
            texturePos = 5;
        }
    }
    else if (texturePos < 4){
        texturePos = 0;
    }
    else{
        texturePos = 4;
    }

    
     
             
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, texture_[texturePos]);
    
    
    glColor4f(0.7f,0.9f,1.0f,1.0f);
    glBegin( GL_QUADS );
    glTexCoord2d(0.0,0.0); glVertex2d(left(),bottom());
    glTexCoord2d(1.0,0.0); glVertex2d(right(),bottom());
    glTexCoord2d(1.0,1.0); glVertex2d(right(),top());
    glTexCoord2d(0.0,1.0); glVertex2d(left(),top());
    glEnd();
    
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}
//------------------------------------------------------------
//constructor for Mario Class
Mario::Mario()
{
    //init the private instance variables to default value
    state_ = SMALL_STATE;
    jumpCount_ = 0;
    starCount_ = 0;
    
    isDead_ = false;
    isInvincible_ = false;
    
    //init the keys
    jumpKey_ = false;
    rightKey_ = false;
    leftKey_ = false;
    sprintKey_ = false;
    fireballKey_ = false;
    
    //Set X and Y velocity
    this->setXVelocity(0.0);
    this->setYVelocity(0.0);
    
    texturePos = 0;
    
    // Mac environment variable for home directory
    char *cHomeDir = NULL;
    
    cHomeDir = getenv("HOME");
    
    // I think Windows uses HOMEPATH
    if (!cHomeDir) {
        cHomeDir = getenv("HOMEPATH");
    }
    string homeDir = cHomeDir;
    string iName;
    homeDir += "/CS330/sprites/mario";
    
    string pos;
    stringstream out;
    
    for (int i = 0; i<8; ++i) {
        stringstream out;
        //Generates Filename
        iName = homeDir;
        out<<i;
        pos = out.str();
        iName += pos;
        iName += ".tex";
        
        FILE *fp = fopen(iName.c_str(), "r");
        unsigned char *texture = new unsigned char[4 * 256 * 256];
        if (fread(texture, sizeof(unsigned char), 4 * 256 * 256, fp)
            != 4* 256 *256) {
            fprintf(stderr, "error reading %s", iName.c_str());
        }
        fclose(fp);
        
        glGenTextures(1, &texture_[i]);
        glBindTexture(GL_TEXTURE_2D, texture_[i]);
        
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );        
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_NEAREST );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                        GL_LINEAR );        
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                        GL_CLAMP );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                        GL_CLAMP );
        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, 256, 256, GL_RGBA,
                          GL_UNSIGNED_BYTE, texture);
        delete [] texture;

    }
    
   
}
//------------------------------------------------------------
//updates Mario's movement info when a button is pushed
void Mario::updateKeyDown(unsigned char button)
{
    if (button == 'a')
    {
        leftKey_ = true;
        //set Mario's Velocity
        if (sprintKey_ == true)
        {
            this->setXVelocity(-2.0);
        }
        else 
        {
            this->setXVelocity(-1.0);
        }
    }
    
    if (button == 'd')
    {
        rightKey_ = true;
        
        //set Mario's velocity
        if (sprintKey_ == true)
        {
            this->setXVelocity(2.0);
        }
        else 
        {
            this->setXVelocity(1.0);
        }
    }
    
    if (button == 'w')
    {
        jumpKey_ = true;
        if (jumpCount_ == 0 && this->getYVelocity() == 0.0) 
        {
            jumpCount_ = 50;
            
            //set Mario's velocity
            this->setYVelocity(2.0);
        }
    }
    
    if (button == 'j')
    {
        sprintKey_ = true;
    }
    
    if (button == 'k')
    {
        fireballKey_ = true;
    }
}
//------------------------------------------------------------
//upadates Mario's info when a button is let up
void Mario::updateKeyUp(unsigned char button)
{
	if (button == 'a') {
		leftKey_ = false;
        if (rightKey_) {
            if (this->getXVelocity() < 0) {
                this->setXVelocity(1.0);
            }
        }
        else {
            this->setXVelocity(0.0);
        }
	}
	if (button == 'w') {
		jumpKey_ = false;
        if (jumpCount_ > 15) {
            jumpCount_ = 15;
        } else {
            jumpCount_ = 0;
            this->setYVelocity(-2.0);
        }
	}
    
	if (button == 'd') {
		rightKey_ = false;
        if (leftKey_) {
            if (this->getXVelocity() > 0) {
                this->setXVelocity(-1.0);
            }
        }
        else {
            this->setXVelocity(0.0);
        }
	}
	if (button == 'j') {
		sprintKey_ = false;
	}
	if (button == 'k') {
		fireballKey_ = false;
	}
}
//------------------------------------------------------------
//method to calculate Marios movement
void Mario::move()
{
    //actually does the movement of Mario
    this->setRight(this->right() + this->getXVelocity());
    this->setLeft(this->left() + this->getXVelocity());
    this->setTop(this->top() + this->getYVelocity());
    this->setBottom(this->bottom() + this->getYVelocity());
}
//------------------------------------------------------------
//Handels all jump cases
void Mario::jump() {
    if (this->jumpCount_ > 0) {
        jumpCount_--;
    } else if (this->getYVelocity() > 0.0) {
        this->setYVelocity(-2.0);
    }
}
//------------------------------------------------------------
//updates Mario for one Frame
void Mario::updateScene()
{
    //depending on whats happening we need to update Mario
    // check the key and move mario according to the button that is being pushed
    //call the check methods to check to see if Mario is running into things
    //this works one frame at a time
    //if Mario jumps decrease the jumpCount_ by 1 every frame
    testSwitch();
    jump();
    move();
    
}
//------------------------------------------------------------
//method that calculate the intersections of Mario and objects
//to see if Mario runs into anything
bool Mario::check()
{
    /*
    //get all the values of the objects so Mario knows what to do 
    // the object
    //check each case for each object
    //object list
    //flag: 1, breakable: 2, nonbreakable: 3, Fireflower: 4, coin: 5
    //Mario: 6, Goomba: 7, Mushroom: 8, Plant: 9, Shell: 10, Star: 11
    //Turtle: 12, EnemyFireball: 13, MarioFireball: 14
    
    int count = 0;;
    
    //the level 
    Level *level = Level::sharedLevel();
    
    //the lists of active objects
    LList moveableList = level->getActiveMovable();
    LList drawableList = level->getActiveDrawable();
    LList blockList = level->getActiveBlocks();
    
    //instance of LListIterator
    LListIterator iter;
    
    //object that will be checked for its type
    Drawable *object;
    
    iter.init(moveableList);
    
    while ((object = iter.next())) 
    {
        if (((this->right() >= object->left() && this->right() <= object->right()) || (this->left() <= object->right() && this->left() >= object->left())) && (this->bottom() <= object->top()))
        {
            //if Mario runs into an enemy
            if (object->objectType() == GOOMBA || object->objectType() == PLANT || object->objectType() == SHELL || object->objectType() == TURTLE || object->objectType() == ENEMYFIREBALL)
            {
                //Mario dies from the right
                //go through the changes of Mario's state based on current state
                //and him getting hit by an enemy
                if (isInvincible_)
                {
                    //Mario kills the enemy
                }
                
                else if (this->state_ == FIRE_STATE)
                {
                  this->state_ = BIG_STATE;
                }
                
                else if (this->state_ == BIG_STATE)
                {
                    this->state_ = SMALL_STATE;
                }
                
                else {
                    //Mario dies
                    return false;
                }
                
            }
            //if Mario runs into a Movable Reward
            else if (object->objectType() == MUSHROOM || object->objectType() == STAR)
            {
                //update Mario's State
                if (object->objectType() == MUSHROOM) 
                {
                    if (this->state_ == SMALL_STATE)
                    {
                        this->state_ = BIG_STATE;
                    }
                    else
                    {
                        //give Mario Points for getting mushroom
                    }
                    
                }
                else if (object->objectType() == STAR)
                {
                    //Mario is invincible 
                    isInvincible_ = true;
                    //Update invincible count
                    starCount_ = 50;
                }
            }
        }
                
        //possibly checking if things fall on Mario's Head
        
         if ((this->top() == object->bottom()) && ((this->right() >= object->left() || this->right() <= object->right()) || (this->left() >= object->left() || this->left() <= object->right())))
         {
             //if an enemy lands on top of Mario
             if (object->objectType() == GOOMBA || object->objectType() == SHELL || object->objectType() == TURTLE || object->objectType() == ENEMYFIREBALL)
             {
                 if (isInvincible_)
                 {
                     //enemy dies
                 }
                 else 
                 {
                     //Mario dies
                     return false;
                 }
             }
             //if a reward lands on top of Mario
             else if (object->objectType() == MUSHROOM || object->objectType() == STAR)
             {
                 //update Mario's State
                 if (object->objectType() == MUSHROOM) 
                 {
                     if (this->state_ == SMALL_STATE)
                     {
                         this->state_ = BIG_STATE;
                     }
                     else
                     {
                         //give Mario Points for getting mushroom
                     }
                     
                 }
                 else if (object->objectType() == STAR)
                 {
                     //Mario is invincible 
                     isInvincible_ = true;
                     //Update invicible count
                     starCount_ = 50;
                 }
             }
         }
        
        
        //checks if mario lands on top of object
        if ((this->bottom() == object->top()) && ((this->right() >= object->left() || this->right() <= object->right()) || (this->left() >= object->left() || this->left() <= object->right()))) 
        {
            //if Mario lands on top of an enemy
            if (object->objectType() == GOOMBA || object->objectType() == SHELL || object->objectType() == TURTLE)
            {
                //enemy dies
                //Mario's jumpcount reset? (Mario will jump higher (bounce))
            }
            //if Mario lands on a reward
            else if (object->objectType() == MUSHROOM || object->objectType() == STAR)
            {
                //update Mario's State
                if (object->objectType() == MUSHROOM) 
                {
                    if (this->state_ == SMALL_STATE)
                    {
                        this->state_ = BIG_STATE;
                    }
                    else
                    {
                        //give Mario Points for getting mushroom
                    }
                    
                }
                else if (object->objectType() == STAR)
                {
                    //Mario is invincible 
                    isInvincible_ = true;
                    //Update invincible count
                    starCount_ = 50;
                }
            }
            //if Mario lands on an enemy that can't be killed
            else if (object->objectType() == PLANT || object->objectType() == ENEMYFIREBALL)
            {
                //go through the changes of Mario's state based on current state
                //and him getting hit by an enemy
                if (this->state_ == FIRE_STATE)
                {
                    this->state_ = BIG_STATE;
                }
                else if (this->state_ == BIG_STATE)
                {
                    this->state_ = SMALL_STATE;
                }
                else {
                    //Mario dies
                    return false;
                }
            }
        }
    }


    iter.init(drawableList);
    
    while ((object = iter.next()))
    {
        if (((this->right() >= object->left() && this->right() <= object->right()) || (this->left() <= object->right() && this->left() >= object->left())) && (this->bottom() <= object->top())) 
        {
            if (object->objectType() == FLAG) 
            {
                //end game
            }
            else if (object->objectType() == FIREFLOWER) 
            {
                //update Mario's State
                if (this->state_ == FIRE_STATE)
                {
                    //generate points for fireflower
                }
                else
                {
                    this->state_ = FIRE_STATE;
                }
                    
            }
            else if(object->objectType() == COIN) 
            {
                //update Mario's Points
            }
        }
    }
    
    iter.init(blockList);
    
    while ((object = iter.next()))
    {
        //check if Mario is jumping into a block
        if ((this->top() >= object->bottom() && this->top() <= object->top()) && ((this->right() >= object->left() && this->right() <= object->right()) || (this->left() >= object->left() && this->left() <= object->right()))) 
        {
            this->jumpCount_ = 0;
            this->setYVelocity(-2.0);
            if (this->state_ == BIG_STATE && object->objectType() == BREAKABLE) {
                //break block
            }
            else 
            {
                //stop moving
                if (object->objectType() == QUESTION)
                {
                    //generate reward
                }
            }
        }
        //check if Mario lands on a block
        if (((this->right() >= object->left() && this->right() <= object->right()) || (this->left() <= object->right() && this->left() >= object->left())) && (this->bottom() <= object->top() && this->bottom() >= object->bottom()))
        {
            //stop falling
            //keep moving
            count = 1;
            if (jumpCount_ == 0) {
                this->setYVelocity(0.0);
            }
        }
    }
    if (count == 0 && this->getYVelocity() == 0) {
        this->setYVelocity(-2.0);
    }
    if (this->bottom() > 0)
        return true;
    else
        return false;
    */
    Drawable *objb, *objt, *objl, *objr;
    //Level* level = Level::sharedLevel();
    //LList* liMoveable = &level->getActiveDrawable();
    
    objb = this->checkBottom();
    objt = this->checkTop();
    objl = this->checkLeft();
    objr = this->checkRight();
    if (!objb && this->getYVelocity() == 0) {
        this->setYVelocity(-2.0);
    }
    //mario jumps into something
    if (objt) {
        if (objt->objectType() == QUESTION) {
            Nonbreakable *temp = (Nonbreakable*)objt;
            temp->generateReward(this->getState() != SMALL_STATE);
        } else if (objt->objectType() == BREAKABLE) {
            Breakable *temp = (Breakable*)objt;
            temp->breakBlock(this->getState() != SMALL_STATE);
        }
        if (objt->objectType() == GOOMBA || objt->objectType() == SHELL || objt->objectType() == TURTLE || objt->objectType() == ENEMYFIREBALL) {
            if (starCount_ > 0) {
                //kill enemy
            }
            else {
                return false;
            }
        }
        if (objt->objectType() == MUSHROOM || objt->objectType() == STAR || objt->objectType() == FIREFLOWER || objt->objectType() == COIN) {
            //update state
            if (objt->objectType() == MUSHROOM) {
                Level::sharedLevel()->getActiveMovable().removeDrawable(objt);
                cout << "mushroom";
                //liMoveable->removeDrawable(objt);
                if (this->state_ == SMALL_STATE) {
                    this->state_ = BIG_STATE;
                }
                else {
                    //add points
                }
            }
            else if (objt->objectType() == FIREFLOWER) {
                if (this->getState() == FIRE_STATE) {
                    //add points
                }
                else {
                    this->state_ = FIRE_STATE;
                }
            }
            else {
                starCount_ = 50;
            }
        }
        //Up above, waiting for Drew
        this->jumpCount_ = 0;
        this->setYVelocity(-2.0);
    }
    //mario falls on something
    if (objb && this->getYVelocity() < 0) {
        this->setYVelocity(0.0);
        if (objb->objectType() == GOOMBA || objb->objectType() == SHELL || objb->objectType() == TURTLE || objb->objectType() == ENEMYFIREBALL || objb->objectType() == PLANT) {
            if (starCount_ > 0) {
                //kill enemy
            }
            else {
                if (objb->objectType() == TURTLE) {
                    //turn turtle into shell
                }
                else if (objb->objectType() == ENEMYFIREBALL || objb->objectType() == PLANT){
                    //Mario Dies
                    return false;
                }
                else {
                    //delete enemy
                }
            }
        }
        else if (objb->objectType() == MUSHROOM || objb->objectType() == STAR || objb->objectType() == FIREFLOWER) {
            //update state
            if (objb->objectType() == MUSHROOM) {
                Level::sharedLevel()->getActiveMovable().removeDrawable(objb);
                cout << "mushroom";
                if (this->state_ == SMALL_STATE) {
                    this->state_ = BIG_STATE;
                }
                else if (objb->objectType() == FIREFLOWER) {
                    this->state_ = FIRE_STATE;
                }
                else {
                    //add points
                }
            }
            else {
                starCount_ = 50;
            }
        }
        else if (objb->objectType() == BREAKABLE || objb->objectType() == REGULAR || objb->objectType() == QUESTION) {
            jumpCount_ = 0;
            this->setYVelocity(0.0);
        }
    }
    //Mario moves to the left
    if (objl && this->getXVelocity() < 0) {
        this->setXVelocity(0.0);
        if (objl->objectType() == GOOMBA || objl->objectType() == SHELL || objl->objectType() == TURTLE || objl->objectType() == ENEMYFIREBALL){
            if (this->starCount_ > 0){
                //enemy dies
            }
            else if (this->getState() == BIG_STATE || this->getState() == FIRE_STATE){
                this->state_--;
            }
            else{
                return false;
            }
        }
        else if (objl->objectType() == BREAKABLE || objl->objectType() == REGULAR || objl->objectType() == QUESTION){
            this->setXVelocity(0.0);
        }
        else if (objl->objectType() == MUSHROOM || objl->objectType() == STAR || objl->objectType() == FIREFLOWER || 
                 objl->objectType() == COIN){
            if (objl->objectType() == MUSHROOM) {
                Level::sharedLevel()->getActiveMovable().removeDrawable(objl);
                cout << "mushroom";
                if (this->getState() == SMALL_STATE) {
                    this->state_ = BIG_STATE;
                }
                else{
                    //mario gets points
                }
            }
            else if (objl->objectType() == FIREFLOWER){
                if (this->getState() == FIRE_STATE) {
                    //mario gets some points
                }
                else {
                    this->state_ = FIRE_STATE;
                }
            }
            else if (objl->objectType() == STAR){
                starCount_ = 50;
            }
            else{
                //Mario get some points
            }
        }
        
    } else if (!objl && leftKey_) {
        this->setXVelocity(-1.0);
    }
    //Mario is moving to the right
    if (objr && this->getXVelocity() > 0) {
        this->setXVelocity(0.0);
        cout << "mushroom";
        if (objr->objectType() == GOOMBA || objr->objectType() == SHELL || objr->objectType() == TURTLE || objr->objectType() == ENEMYFIREBALL){
            if (this->starCount_ > 0){
                //enemy dies
            }
            else if (this->getState() == BIG_STATE || this->getState() == FIRE_STATE){
                this->state_--;
            }
            else{
                return false;
            }
        }
        else if (objr->objectType() == BREAKABLE || objr->objectType() == REGULAR || objr->objectType() == QUESTION){
            this->setXVelocity(0.0);
        }
        else if (objr->objectType() == MUSHROOM || objr->objectType() == STAR || objr->objectType() == FIREFLOWER || objr->objectType() == COIN){
            if (objr->objectType() == MUSHROOM) {
                Level::sharedLevel()->getActiveMovable().removeDrawable(objr);
                cout << "mushroom";
                if (this->getState() == SMALL_STATE) {
                    this->state_ = BIG_STATE;
                }
                else{
                    //mario gets points
                }
            }
            else if (objr->objectType() == FIREFLOWER){
                if (this->getState() == FIRE_STATE) {
                    //mario gets some points
                }
                else {
                    this->state_ = FIRE_STATE;
                }
            }
            else if (objr->objectType() == STAR){
                starCount_ = 50;
            }
            else{
                //Mario get some points
            }
        }
    } else if (!objr && rightKey_) {
        if (sprintKey_) {
            this->setXVelocity(2.0);
        } else {
            this->setXVelocity(1.0);
        }
    }
    //Mario Falls off the screen
    if (this->top() <= 0)
    {
        isDead_ = true;
        return false;
    }
    //Mario's left bound so he cant move left past screen
    if (this->left() < leftBound_ && this->getXVelocity() < 0)
    {
        this->setXVelocity(0.0);
    }
    return true;
}
//------------------------------------------------------------
//Creates a fireball
bool Mario::fireball()
{
	return false;
}
//------------------------------------------------------------
void Mario::setLeftBound(int leftBound)
{
    leftBound_ = leftBound;
}
//-------------------------------------------------------------
void Mario::testSwitch() {
    Drawable *objb, *objt, *objl, *objr;
    
    objb = this->checkBottom();
    objt = this->checkTop();
    objl = this->checkLeft();
    objr = this->checkRight();
    
    //All items that can hit Mario from the top
    if (objt)
        switch (objt->objectType()) {
            case REGULAR:
                if (this->getYVelocity() > 0) {
                    this->setXVelocity(0.0);
                    this->jumpCount_ = 0;
                }
                break;
            case QUESTION:
                if (this->getYVelocity() > 0) {
                    this->setXVelocity(0.0);
                    this->jumpCount_ = 0;
                }
                ((Nonbreakable*)objt)->generateReward(this->getState() != SMALL_STATE);
                break;
            case BREAKABLE:
                if (this->getYVelocity() > 0) {
                    this->setXVelocity(0.0);
                    this->jumpCount_ = 0;
                }
                ((Breakable*) objt)->breakBlock(this->getState() != SMALL_STATE);
                break;
            case GOOMBA:
            case SHELL:
            case ENEMYFIREBALL:
            case TURTLE:
                //die
                break;
            case MUSHROOM:
                break;
            case STAR:
                break;
            case FIREFLOWER:
                break;
            case COIN:
                break;
                
        }
    //All objects that can hit Mario from the bottom
    if (objb) {
        switch (objb->objectType()) {
            case BREAKABLE:
            case REGULAR:
            case QUESTION:
                if (this->getYVelocity() < 0) {
                    this->setYVelocity(0.0);
                }
                break;
            case GOOMBA:
            case SHELL:
            case ENEMYFIREBALL:
            case TURTLE:
                //kill enemy
                break;
            case MUSHROOM:
                break;
            case STAR:
                break;
            case FIREFLOWER:
                break;
            case COIN:
                break;
        }
    } else {
        if (this->getYVelocity() == 0.0) {
            this->setYVelocity(-2.0);
        }
    }
    //All objects that can hit Mario from the left
    if (objl) {
        switch (objl->objectType()) {
            case BREAKABLE:
            case REGULAR:
            case QUESTION:
                if (this->getXVelocity() < 0) {
                    this->setXVelocity(0.0);
                }
                break;
            case GOOMBA:
            case SHELL:
            case ENEMYFIREBALL:
            case TURTLE:
                //kill enemy
                break;
            case MUSHROOM:
                break;
            case STAR:
                break;
            case FIREFLOWER:
                break;
            case COIN:
                break;
                
        }
    } else if (rightKey_) {
        if (sprintKey_) {
            this->setXVelocity(1.2);
        } else {
            this->setXVelocity(1.0);
        }
    }
    //All objects that can hit Mario from the right
    if (objr) {
        switch (objr->objectType()) {
            case BREAKABLE:
            case REGULAR:
            case QUESTION:
                if (this->getXVelocity() > 0) {
                    this->setXVelocity(0.0);
                }
                break;
            case GOOMBA:
            case SHELL:
            case ENEMYFIREBALL:
            case TURTLE:
                //kill enemy
                break;
            case MUSHROOM:
                break;
            case STAR:
                break;
            case FIREFLOWER:
                break;
            case COIN:
                break;
                
        }
    } else if (leftKey_) {
        if (sprintKey_) {
            this->setXVelocity(-1.2);
        } else {
            this->setXVelocity(-1.0);
        }
    }
    //stops Mario moving out of the left bound
    if (this->left() < leftBound_ && this->getXVelocity() < 0)
    {
        this->setXVelocity(0.0);
    }
}
