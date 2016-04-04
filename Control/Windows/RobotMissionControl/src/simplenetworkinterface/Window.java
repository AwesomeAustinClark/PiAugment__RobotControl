/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package simplenetworkinterface;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Graphics2D;
import java.awt.event.WindowEvent;
import java.io.IOException;
import java.net.InetAddress;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JToggleButton;
import net.java.games.input.Component;
import net.java.games.input.Controller;
import net.java.games.input.Rumbler;


/**
 *
 * @author Grady
 */
public class Window extends javax.swing.JFrame{
    public Thread sendReceiveThread;
    public easySocket server;
    public int motorFrontLeft = 0;
    public int motorFrontRight = 0;
    public int motorBackLeft = 0;
    public int motorBackRight = 0;
    
    //Keyboard
    public int nws = 0;
    public int nad = 0;
    public boolean exitBothSides = false;
    public boolean US_override = false;
    
    
    InetAddress IP;
    int PORT;
    
    void setIP(InetAddress ip){
        IP = ip;
        this.ipField.setText("IP: "+IP+" | Port: "+PORT);
    }
    
    void setPort(int port_){
        PORT = port_;
        this.ipField.setText("IP: "+IP+" | Port: "+PORT);
    }
            
    /**
     * Creates new form Window
     */
    public Window(){
        initComponents();
        byte b = '!';
    }
    
    byte setBit(byte bt, boolean offOn, int atPos){
        if(offOn){
            return (byte) (bt | (1 << atPos));   
        }else{
            return (byte) (bt & ~(1 << atPos));
        }
    }
    
    byte[] makeStringToSend(int mfl, int mfr, int mbl, int mbr, boolean override, boolean servo, boolean servo2, boolean servo3){ // values_in are -100 to +100
        int ival = 0;
        int ival2 = 0;
        int ival3 =0;
        int ival4 =0;
        byte val = 0;
        byte val2 = 0;
        byte val3 = 0;
        byte val4 = 0;
        byte modifiers = 0;
        byte ts[] = new byte[7];
        /*
            "[01234]"
            [1:mfl,2:mfr,3:mbl,4:mbr]
        */        
        // byte are 0 to 255, and half of 255 is 127.5
        ival = (int)(((Math.abs(mfl))/(double)100)*255);
        val = (byte)ival; 
        modifiers = setBit(modifiers, mfl>=0, 0);
        
        ival2 = (int)((Math.abs(mfr)/(double)100)*255);
        val2 = (byte)ival2; 
        modifiers = setBit(modifiers, mfr>=0, 1);
        
        ival3 = (int)((Math.abs(mbl)/(double)100)*255);
        val3 = (byte)ival3;
        modifiers = setBit(modifiers, mbl>=0, 2);
        
        ival4 = (int)((Math.abs(mbr)/(double)100)*255);
        val4 = (byte)ival4; 
        modifiers = setBit(modifiers, mbr>=0, 3);
        
        modifiers = setBit(modifiers, servo, 4);
        modifiers = setBit(modifiers, servo2, 5);
        modifiers = setBit(modifiers, servo3, 6);
        
        modifiers = setBit(modifiers, override, 7);
        
        
        ts[0]= '[';
        ts[1]= modifiers;
        ts[2]= val;
        ts[3]= val2;
        ts[4]= val3;
        ts[5]= val4;
        ts[6]= ']';
        //System.out.println(ival+" : "+val+" : "+String.format("%8s", Integer.toBinaryString(val & 0xFF)).replace(' ', '0'));
        //System.out.println(ival+","+ival2+","+ival3+","+ival4);
        return (ts);
        //"["+(char)(val)+""+(char)(val2)+""+(char)(val3)+""+(char)(val4)+"]"
    }
    
    
    // Tankdrive function, that converts the x and y values
    // to store as left/right
    public double[] tankdrive(double x,double y, double left,double right) {
        // x is -50 to 50
        // y is -50 to 50
        right = (y/50)*100;
        left =  (x/50)*100;
        
        //System.out.print(left+" : "+right);
        return new double[]{left,right};
    }
    
    public void setup() throws IOException{
        server = new easySocket();
        server.setup(PORT,IP,PORT);
        final Window win = this;
        gamepadThread = new Thread(new Runnable() {
            @Override
            public void run() {
                GamePadControl.setup();
                while(!exitBothSides){
                    GamePadControl.searchForControllers();
                    if(!GamePadControl.foundControllers.isEmpty()){
                        for(int i=0;i<GamePadControl.foundControllers.size();++i){
                            addControllerName(GamePadControl.foundControllers.get(i).toString());   
                        }
                        ShowControllerData(win);
                    }else{
                        GamePadComboBox.removeAllItems();
                        GamePadComboBox.addItem("No Controller!");
                        try {
                            Thread.sleep(500);
                        } catch (InterruptedException ex) {
                            Logger.getLogger(Window.class.getName()).log(Level.SEVERE, null, ex);
                        }
                    } 
                }
                              
            }
        });
        gamepadThread.start();
        sendReceiveThread = new Thread(new Runnable(){
            @Override
            public void run() {
                while(!exitBothSides){
                    try {
                        int mfl = 0;
                        int mfr = 0;
                        int mbl = 0;
                        int mbr = 0;
                        int ct = captureType.getSelectedIndex();
                        if(ct==1){ // Keyboard
                            if(nws==0 && nad==0){
                                commandField.setText("STOP");
                                mfl = 0;
                                mfr = 0;  
                                mbl = 0;
                                mbr = 0;
                            }else if(nws==1 && nad==0){
                                commandField.setText("Forward");
                                mfl = 100;
                                mfr = 100;  
                                mbl = 100;
                                mbr = 100;
                            }else if(nws==2 && nad==0){
                                commandField.setText("Backward");
                                mfl = -100;
                                mfr = -100;  
                                mbl = -100;
                                mbr = -100;
                            }else if(nws==0 && nad==1){
                                commandField.setText("Turning Left");
                                mfl = -100;
                                mfr = +100;  
                                mbl = -100;
                                mbr = +100;
                            }else if(nws==1 && nad==1){
                                commandField.setText("Forward Left");
                                mfl = 0;
                                mfr = 100;  
                                mbl = 0;
                                mbr = 100;
                            }else if(nws==2 && nad==1){
                                commandField.setText("Backward Left");
                                mfl = 0;
                                mfr = -100;  
                                mbl = 0;
                                mbr = -100;
                            }else if(nws==0 && nad==2){
                                commandField.setText("Right");
                                mfl = 100;
                                mfr = -100;  
                                mbl = 100;
                                mbr = -100;
                            }else if(nws==1 && nad==2){
                                commandField.setText("Forward Right");
                                mfl = 100;
                                mfr = 0;  
                                mbl = 100;
                                mbr = 0;
                            }else if(nws==2 && nad==2){
                                commandField.setText("Backward Right");
                                mfl = 100;
                                mfr = 0;  
                                mbl = 100;
                                mbr = 0;
                            }else if(nws==-1 && nad==-1){
                                commandField.setText("SHUTDOWN");
                                break;
                            }else{
                                commandField.setText("!STOP!");
                                mfl = 0;
                                mfr = 0;  
                                mbl = 0;
                                mbr = 0;
                            }
                            server.send(makeStringToSend(mfl, mfr, mbl, mbr, US_override,false,false,false));
                        }else if(ct == 2){// Gamepad
                            double x = ryAxisPercentage-50;
                            double y = yAxisPercentage-50;
                            double right = 0;
                            double left = 0;
                            double cat[];
                            /*
                                static int xAxisPercentage = 0;
                                static int yAxisPercentage = 0; 
                                static float hatSwitchPosition = 0;
                                static float axisValue = 0;
                                static int axisValueInPercentage = 0;
                            */
                            //cat = tankdrive(x, y, left, right);
                            //left = cat[1];
                            //right = cat[0];
                            left = (x/50)*100;
                            right = (y/50)*100;
                            //left*=-1;
                            //right*=-1;
                            // DEADZONE
                            if(left>=-10 && left<=10){left=0;}
                            if(right>=-10 && right<=10){right=0;}
                            // DEADZONE
                            //System.out.println(" <> "+right+" : "+left);
                            //System.out.println("Left: "+left+" Right: "+right);
                            server.send(makeStringToSend((int)left, (int)right, (int)left, (int)right, US_override, buttonsToggle[0],buttonsToggle[1],buttonsToggle[2]));

                            
                        }
                        //System.out.println(toSend);
                        
                        
                        /*
                        DatagramPacket packet = server.read();
                        if(packet!=null){
                            System.out.println("IN: "+Arrays.toString(packet.getData()));
                            if(packet.getAddress()==server.remoteIP && packet.getPort() == server.remotePort){
                                //byte[] data = packet.getData();
                                // WIP
                            }
                        }
                        */
                        if(buttons[12]){
                            exitWindow();
                        }
                        Thread.sleep(100);
                    } catch (InterruptedException ex) {
                        Logger.getLogger(Window.class.getName()).log(Level.SEVERE, null, ex);
                    } catch (IOException ex) {
                        Logger.getLogger(Window.class.getName()).log(Level.SEVERE, null, ex);
                    }
                }
                try {
                    server.send("[-]");
                    server.send("[-]");
                    server.send("[-]");
                    server.send("[-]");
                    server.send("[-]");
                    server.send("[-]");
                    server.send("[-]");
                    server.send("[-]");
                    server.send("[-]");
                    server.send("[-]");
                    server.send("[-]");
                } catch (IOException ex) {
                    Logger.getLogger(Window.class.getName()).log(Level.SEVERE, null, ex);
                }
                server.close();       
            }
            
        });
        sendReceiveThread.start();
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        connectionField = new javax.swing.JScrollPane();
        ipField = new javax.swing.JTextPane();
        jScrollPane2 = new javax.swing.JScrollPane();
        connectionStatusField = new javax.swing.JTextPane();
        jScrollPane3 = new javax.swing.JScrollPane();
        commandField = new javax.swing.JTextPane();
        connectionBar = new javax.swing.JProgressBar();
        GamePadComboBox = new javax.swing.JComboBox<>();
        jScrollPane1 = new javax.swing.JScrollPane();
        controlField = new javax.swing.JTextArea();
        ckField = new javax.swing.JTextField();
        axisXYPanel = new javax.swing.JPanel();
        captureType = new javax.swing.JComboBox<>();
        axisForPanel = new javax.swing.JPanel();
        ButtonPanel = new javax.swing.JPanel();
        hatSwitchPanel = new javax.swing.JPanel();
        axisForlabel = new javax.swing.JLabel();
        axisLabel = new javax.swing.JLabel();
        GamepadSupported = new javax.swing.JTextField();
        Cb_US_Override = new javax.swing.JCheckBox();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setResizable(false);
        addWindowListener(new java.awt.event.WindowAdapter() {
            public void windowClosed(java.awt.event.WindowEvent evt) {
                formWindowClosed(evt);
            }
            public void windowClosing(java.awt.event.WindowEvent evt) {
                formWindowClosing(evt);
            }
        });

        ipField.setEditable(false);
        ipField.setText("Pi: 0.0.0.0  Port: 0 | This: 0.0.0.0 Port: 0");
        connectionField.setViewportView(ipField);

        connectionStatusField.setEditable(false);
        connectionStatusField.setText("status: BAD   last response: 999999999 ");
        jScrollPane2.setViewportView(connectionStatusField);

        commandField.setEditable(false);
        commandField.setText("Forward, Turn Right, Turn Left, Backward, Forward right, Forward left, Backward right, Backward left, EMERGENCY STOP");
        commandField.setToolTipText("");
        jScrollPane3.setViewportView(commandField);

        controlField.setEditable(false);
        controlField.setColumns(20);
        controlField.setFont(new java.awt.Font("Monospaced", 1, 20)); // NOI18N
        controlField.setRows(5);
        controlField.setText("Move mouse over this box and press,\nw = go forward\na = turn left\ns = go backwards\nd = go right\nw + a/d = forward + turn in said direction\ns + a/d = backwards + turn in said direction\nbackspace or cancel button = exit both sides");
        controlField.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyPressed(java.awt.event.KeyEvent evt) {
                controlFieldKeyPressed(evt);
            }
            public void keyReleased(java.awt.event.KeyEvent evt) {
                controlFieldKeyReleased(evt);
            }
        });
        jScrollPane1.setViewportView(controlField);

        ckField.setEditable(false);
        ckField.setText("CAPTURING:");
        ckField.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                ckFieldActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout axisXYPanelLayout = new javax.swing.GroupLayout(axisXYPanel);
        axisXYPanel.setLayout(axisXYPanelLayout);
        axisXYPanelLayout.setHorizontalGroup(
            axisXYPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 268, Short.MAX_VALUE)
        );
        axisXYPanelLayout.setVerticalGroup(
            axisXYPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 220, Short.MAX_VALUE)
        );

        captureType.setModel(new javax.swing.DefaultComboBoxModel<>(new String[] { "Do not send", "Keyboard", "Gamepad" }));

        javax.swing.GroupLayout axisForPanelLayout = new javax.swing.GroupLayout(axisForPanel);
        axisForPanel.setLayout(axisForPanelLayout);
        axisForPanelLayout.setHorizontalGroup(
            axisForPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 258, Short.MAX_VALUE)
        );
        axisForPanelLayout.setVerticalGroup(
            axisForPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 0, Short.MAX_VALUE)
        );

        javax.swing.GroupLayout ButtonPanelLayout = new javax.swing.GroupLayout(ButtonPanel);
        ButtonPanel.setLayout(ButtonPanelLayout);
        ButtonPanelLayout.setHorizontalGroup(
            ButtonPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 268, Short.MAX_VALUE)
        );
        ButtonPanelLayout.setVerticalGroup(
            ButtonPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 198, Short.MAX_VALUE)
        );

        javax.swing.GroupLayout hatSwitchPanelLayout = new javax.swing.GroupLayout(hatSwitchPanel);
        hatSwitchPanel.setLayout(hatSwitchPanelLayout);
        hatSwitchPanelLayout.setHorizontalGroup(
            hatSwitchPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 0, Short.MAX_VALUE)
        );
        hatSwitchPanelLayout.setVerticalGroup(
            hatSwitchPanelLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 0, Short.MAX_VALUE)
        );

        GamepadSupported.setEditable(false);

        Cb_US_Override.setText("Ultrsonic sensor Override");
        Cb_US_Override.addItemListener(new java.awt.event.ItemListener() {
            public void itemStateChanged(java.awt.event.ItemEvent evt) {
                Cb_US_OverrideItemStateChanged(evt);
            }
        });

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                        .addGroup(layout.createSequentialGroup()
                            .addComponent(connectionField, javax.swing.GroupLayout.PREFERRED_SIZE, 236, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                            .addComponent(connectionBar, javax.swing.GroupLayout.PREFERRED_SIZE, 231, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addGap(82, 82, 82))
                        .addGroup(javax.swing.GroupLayout.Alignment.LEADING, layout.createSequentialGroup()
                            .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                                .addGroup(javax.swing.GroupLayout.Alignment.LEADING, layout.createSequentialGroup()
                                    .addComponent(ckField, javax.swing.GroupLayout.PREFERRED_SIZE, 77, javax.swing.GroupLayout.PREFERRED_SIZE)
                                    .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                    .addComponent(captureType, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                                    .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                    .addComponent(Cb_US_Override))
                                .addGroup(javax.swing.GroupLayout.Alignment.LEADING, layout.createSequentialGroup()
                                    .addComponent(jScrollPane2, javax.swing.GroupLayout.PREFERRED_SIZE, 215, javax.swing.GroupLayout.PREFERRED_SIZE)
                                    .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                                    .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                                        .addComponent(GamePadComboBox, 0, 260, Short.MAX_VALUE)
                                        .addComponent(GamepadSupported))))
                            .addGap(74, 74, 74)))
                    .addGroup(layout.createSequentialGroup()
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(jScrollPane3, javax.swing.GroupLayout.PREFERRED_SIZE, 455, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(jScrollPane1, javax.swing.GroupLayout.PREFERRED_SIZE, 536, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)))
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(ButtonPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(hatSwitchPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                    .addGroup(layout.createSequentialGroup()
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(axisXYPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(axisLabel, javax.swing.GroupLayout.PREFERRED_SIZE, 76, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(axisForPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(axisForlabel, javax.swing.GroupLayout.PREFERRED_SIZE, 82, javax.swing.GroupLayout.PREFERRED_SIZE))))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(connectionField, javax.swing.GroupLayout.PREFERRED_SIZE, 34, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(connectionBar, javax.swing.GroupLayout.PREFERRED_SIZE, 23, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                        .addGap(6, 6, 6)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                            .addGroup(layout.createSequentialGroup()
                                .addComponent(GamepadSupported, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                                .addComponent(GamePadComboBox, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                            .addComponent(jScrollPane2, javax.swing.GroupLayout.PREFERRED_SIZE, 40, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addGap(18, 18, 18)
                        .addComponent(jScrollPane3, javax.swing.GroupLayout.PREFERRED_SIZE, 47, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(ckField, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(captureType, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(Cb_US_Override))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jScrollPane1, javax.swing.GroupLayout.PREFERRED_SIZE, 275, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(layout.createSequentialGroup()
                        .addGap(20, 20, 20)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(axisForlabel, javax.swing.GroupLayout.PREFERRED_SIZE, 17, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(axisLabel, javax.swing.GroupLayout.PREFERRED_SIZE, 17, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                            .addComponent(axisForPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(axisXYPanel, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                            .addComponent(ButtonPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(hatSwitchPanel, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))))
                .addContainerGap(25, Short.MAX_VALUE))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents
    
    private void formWindowClosed(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_formWindowClosed
        
    }//GEN-LAST:event_formWindowClosed

    private void controlFieldKeyPressed(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_controlFieldKeyPressed
        char key = evt.getKeyChar();
        //System.out.println(evt.getExtendedKeyCode()+" : "+key);
        switch (key) {
            case 'w':
                nws = 1;
                break;
            case 'a':
                nad = 1;
                break;
            case 's':
                nws = 2;
                break;
            case 'd':
                nad = 2;
                break;
            default:
                break;
        }
    }//GEN-LAST:event_controlFieldKeyPressed

    private void controlFieldKeyReleased(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_controlFieldKeyReleased
        if(evt.getKeyChar() == '\b'){
            dispatchEvent(new WindowEvent(this, WindowEvent.WINDOW_CLOSING));
        }
        char key = evt.getKeyChar();
        switch (key) {
            case 'w':
                nws = 0;
                break;
            case 'a':
                nad = 0;
                break;
            case 's':
                nws = 0;
                break;
            case 'd':
                nad = 0;
                break;
            default:
                break;
        }
    }//GEN-LAST:event_controlFieldKeyReleased

    private void ckFieldActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_ckFieldActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_ckFieldActionPerformed

    private void formWindowClosing(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_formWindowClosing
        exitBothSides = true;
        try {
            Thread.sleep(250);
        } catch (InterruptedException ex) {
            Logger.getLogger(Window.class.getName()).log(Level.SEVERE, null, ex);
        }
        try {
            if(gamepadThread!=null && gamepadThread.isAlive()){
                gamepadThread.join(1000);
            }
            if(sendReceiveThread!=null && sendReceiveThread.isAlive()){
                sendReceiveThread.join(1000);
            }
        } catch (InterruptedException ex) {
            Logger.getLogger(Window.class.getName()).log(Level.WARNING, null, ex);
        }
    }//GEN-LAST:event_formWindowClosing

    private void Cb_US_OverrideItemStateChanged(java.awt.event.ItemEvent evt) {//GEN-FIRST:event_Cb_US_OverrideItemStateChanged
        US_override = !US_override;
    }//GEN-LAST:event_Cb_US_OverrideItemStateChanged
    
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JPanel ButtonPanel;
    public javax.swing.JCheckBox Cb_US_Override;
    public javax.swing.JComboBox<String> GamePadComboBox;
    public javax.swing.JTextField GamepadSupported;
    private javax.swing.JPanel axisForPanel;
    private javax.swing.JLabel axisForlabel;
    private javax.swing.JLabel axisLabel;
    private javax.swing.JPanel axisXYPanel;
    public javax.swing.JComboBox<String> captureType;
    private javax.swing.JTextField ckField;
    public javax.swing.JTextPane commandField;
    private javax.swing.JProgressBar connectionBar;
    private javax.swing.JScrollPane connectionField;
    public javax.swing.JTextPane connectionStatusField;
    public javax.swing.JTextArea controlField;
    private javax.swing.JPanel hatSwitchPanel;
    public javax.swing.JTextPane ipField;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JScrollPane jScrollPane3;
    // End of variables declaration//GEN-END:variables
   
    public void addControllerName(String msg){
        this.GamePadComboBox.addItem(msg);
    }
    
    public void showControllerDisconnected(){
        this.GamePadComboBox.removeAllItems();
        this.GamePadComboBox.addItem("Controller disconnected!");
    }
    
    public int getSelectedControllerName(){
        return GamePadComboBox.getSelectedIndex();
    }
    
    public void setXYAxis(int xPercentage, int yPercentage){
        Graphics2D g2d = (Graphics2D)axisXYPanel.getGraphics();
        g2d.clearRect(1, 1, axisXYPanel.getWidth() - 2, axisXYPanel.getHeight() - 2);
        g2d.fillOval(xPercentage, yPercentage, 10, 10);
    }
    
    public void setControllerButtons(JPanel buttonsPanel){
        ButtonPanel.removeAll();
        ButtonPanel.add(buttonsPanel);
        ButtonPanel.validate();
    }
    
    public void addAxisPanel(javax.swing.JPanel axesPanel){
        axisForPanel.removeAll();
        axisForPanel.add(axesPanel);
        axisForPanel.validate();
    }
    
    int hatSwitchPos=0;
    public void setHatSwitch(float hatSwitchPosition) {
        int circleSize = 100;
        Graphics2D g2d = (Graphics2D)hatSwitchPanel.getGraphics();
        g2d.clearRect(5, 15, hatSwitchPanel.getWidth() - 10, hatSwitchPanel.getHeight() - 22);
        g2d.drawOval(20, 22, circleSize, circleSize);
        
        if(Float.compare(hatSwitchPosition, Component.POV.OFF) == 0)
            return;
        
        int smallCircleSize = 10;
        int upCircleX = 65;
        int upCircleY = 17;
        int leftCircleX = 15;
        int leftCircleY = 68;
        int betweenX = 37;
        int betweenY = 17;
        
        int x = 0;
        int y = 0;
        
        g2d.setColor(Color.blue);
                        
        if(Float.compare(hatSwitchPosition, Component.POV.UP) == 0){
            x = upCircleX;
            y = upCircleY;
            hatSwitchPos = 0;
        }else if(Float.compare(hatSwitchPosition, Component.POV.DOWN) == 0){
            x = upCircleX;
            y = upCircleY + circleSize;
            hatSwitchPos = 4;
        }else if(Float.compare(hatSwitchPosition, Component.POV.LEFT) == 0){
            x = leftCircleX;
            y = leftCircleY;
            hatSwitchPos = 6;
        }else if(Float.compare(hatSwitchPosition, Component.POV.RIGHT) == 0){
            x = leftCircleX + circleSize;
            y = leftCircleY;
            hatSwitchPos = 2;
        }else if(Float.compare(hatSwitchPosition, Component.POV.UP_LEFT) == 0){
            x = upCircleX - betweenX;
            y = upCircleY + betweenY;
            hatSwitchPos = 7;
        }else if(Float.compare(hatSwitchPosition, Component.POV.UP_RIGHT) == 0){
            x = upCircleX + betweenX;
            y = upCircleY + betweenY;
            hatSwitchPos = 1;
        }else if(Float.compare(hatSwitchPosition, Component.POV.DOWN_LEFT) == 0){
            x = upCircleX - betweenX;
            y = upCircleY + circleSize - betweenY;
            hatSwitchPos = 5;
        }else if(Float.compare(hatSwitchPosition, Component.POV.DOWN_RIGHT) == 0){
            x = upCircleX + betweenX;
            y = upCircleY + circleSize - betweenY;
            hatSwitchPos = 3;
        }
        
        g2d.fillOval(x, y, smallCircleSize, smallCircleSize);
    }
    
    void exitWindow(){
        dispatchEvent(new WindowEvent(this, WindowEvent.WINDOW_CLOSING));
    }
    
    // X axis and Y axis
    int xAxisPercentage = 0;
    int yAxisPercentage = 0; 
    int lyAxisPercentage = 0;
    int ryAxisPercentage = 0; 
    float hatSwitchPosition = 0;
    float axisValue = 0;
    int axisValueInPercentage = 0;
    boolean buttons[] = new boolean[30];
    boolean buttonsPressed[] = new boolean[30];
    boolean buttonsToggle[] = new boolean[30];
    public void ShowControllerData(Window win){
        while(!exitBothSides){
            // Currently selected controller.
            int selectedControllerIndex = win.getSelectedControllerName();
            Controller controller = GamePadControl.foundControllers.get(selectedControllerIndex);
                
            // Pull controller for current data, and break while loop if controller is disconnected.
            if( !controller.poll() ){
                win.showControllerDisconnected();
                break;
            }
            /*
            Rumbler Rumblers[] =  controller.getRumblers();
            System.out.println(Rumblers.length);
            if(Rumblers.length>0){
                Rumblers[0].rumble(1);
            }
            */
            
            // JPanel for other axes.
            JPanel axesPanel = new JPanel(new FlowLayout(FlowLayout.LEFT, 25, 2));
            axesPanel.setBounds(0, 0, 200, 190);
            
            // JPanel for controller buttons
            JPanel buttonsPanel = new JPanel(new FlowLayout(FlowLayout.LEFT, 1, 1));
            buttonsPanel.setBounds(6, 19, 246, 110);
                    
            // Go trough all components of the controller.
            Component[] components = controller.getComponents();
            
            for(int i=0; i < components.length; i++)
            {
                Component component = components[i];
                Component.Identifier componentIdentifier = component.getIdentifier();
                // Buttons
                //if(component.getName().contains("Button")){ // If the language is not english, this won't work.
                if(componentIdentifier.getName().matches("^[0-9]*$")){ // If the component identifier name contains only numbers, then this is a button.
                    // Is button pressed?
                    boolean isItPressed = true;
                    if(component.getPollData() == 0.0f)
                        isItPressed = false;
                    
                    if(isItPressed==false && buttonsPressed[Integer.parseInt(componentIdentifier.getName())]==true){
                        if(Integer.parseInt(componentIdentifier.getName())==3){
                            Cb_US_Override.doClick();
                        }
                        buttonsToggle[Integer.parseInt(componentIdentifier.getName())] = !buttonsToggle[Integer.parseInt(componentIdentifier.getName())];
                    }
                    buttons[Integer.parseInt(componentIdentifier.getName())] = isItPressed;
                    buttonsPressed[Integer.parseInt(componentIdentifier.getName())] = isItPressed;
                    
                    // Button index
                    String buttonIndex;
                    buttonIndex = component.getIdentifier().toString();
                    
                    // Create and add new button to panel.
                    JToggleButton aToggleButton = new JToggleButton(buttonIndex, isItPressed);
                    aToggleButton.setPreferredSize(new Dimension(48, 25));
                    aToggleButton.setEnabled(false);
                    buttonsPanel.add(aToggleButton);
                    
                    // We know that this component was button so we can skip to next component.
                    continue;
                }
                
                // Hat switch
                if(componentIdentifier == Component.Identifier.Axis.POV){
                    hatSwitchPosition = component.getPollData();
                    win.setHatSwitch(hatSwitchPosition);
                    
                    // We know that this component was hat switch so we can skip to next component.
                    continue;
                }
                
                // Axes
                if(component.isAnalog()){
                    axisValue= component.getPollData();
                    axisValueInPercentage = GamePadControl.getAxisValueInPercentage(axisValue);
                    
                    // X axis
                    if(componentIdentifier == Component.Identifier.Axis.X){
                        xAxisPercentage = axisValueInPercentage;
                        
                        continue; // Go to next component.
                    }
                    // Y axis
                    if(componentIdentifier == Component.Identifier.Axis.Y){
                        yAxisPercentage = axisValueInPercentage;
                        continue; // Go to next component.
                    }
                    if("Controller (XBOX 360 For Windows)".equals(controller.getName())){
                        if("y".equals(component.getIdentifier().toString())){
                            lyAxisPercentage = axisValueInPercentage;
                        }
                        if("ry".equals(component.getIdentifier().toString())){
                            ryAxisPercentage = axisValueInPercentage;
                        }
                        GamepadSupported.setText("Gamepad Supported");
                    }else if("BDA GP1".equals(controller.getName())){
                        if("z".equals(component.getIdentifier().toString())){
                            lyAxisPercentage = axisValueInPercentage;
                        }
                        if("rz".equals(component.getIdentifier().toString())){
                            ryAxisPercentage = axisValueInPercentage;
                        }
                        GamepadSupported.setText("Gamepad Supported");
                    }else{
                        if("y".equals(component.getIdentifier().toString())){
                            lyAxisPercentage = axisValueInPercentage;
                        }
                        if("ry".equals(component.getIdentifier().toString())){
                            ryAxisPercentage = axisValueInPercentage;
                        }
                        GamepadSupported.setText("Gamepad Unsupported, milage may vary");
                    }
                    
                    // Other axis
                    JLabel progressBarLabel = new JLabel(component.getName());
                    JProgressBar progressBar = new JProgressBar(0, 100);
                    progressBar.setValue(axisValueInPercentage);
                    axesPanel.add(progressBarLabel);
                    axesPanel.add(progressBar);
                    //System.out.println(component.getIdentifier().toString()+": "+axisValueInPercentage);
                }
            }
            
            
            // Now that we go trough all controller components,
            // we add butons panel to window,
            win.setControllerButtons(buttonsPanel);
            // set x and y axes,
            win.setXYAxis(xAxisPercentage, yAxisPercentage);
            // add other axes panel to window.
            win.addAxisPanel(axesPanel);
            // We have to give processor some rest.
            try {
                Thread.sleep(25);
            } catch (InterruptedException ex) {
                Logger.getLogger(Window.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
    
    
    public void test() throws IOException{
        // take int (32bit) and compress it to 1 byte
        //System.out.println("["+((char)((byte)(33)))+"]");
        //test
        //int i = (int)(((2)/(double)100)*93);
        //System.out.println("["+((char)((byte)(i)))+"]");
        easySocket es = new easySocket();
        es.setup(4000, InetAddress.getLocalHost(), 4000);
        //es.send("Connect");
        System.out.println(es.read(1000));
    }
    
    Thread gamepadThread;
    public static void main(String args[]) throws IOException, InterruptedException {
        //test();
        GetIPPort getWin = new GetIPPort();
        Window window = new Window();
        /*
        window.makeStringToSend(-100, -80, -50, 0);
        window.makeStringToSend(100, 50, 80, 100);
        window.makeStringToSend(-50, -80, -50, 0);
        window.makeStringToSend(50, 50, 80, 100);
        window.makeStringToSend(0, -80, -50, 0);
        */
        /*
        
                try {
                // WIP
                server.send("[@]");
                server.send("[@]");
                server.send("[@]");
                
                } catch (IOException ex) {
                    Logger.getLogger(Window.class.getName()).log(Level.SEVERE, null, ex);
                }
        */
        getWin.setVisible(true);
        while(getWin.initialized==false){
            Thread.sleep(100);
        }
        if(getWin.exit){
            window.dispatchEvent(new WindowEvent(window, WindowEvent.WINDOW_CLOSING));
            getWin.dispatchEvent(new WindowEvent(getWin, WindowEvent.WINDOW_CLOSING));
            return;
        }
        
        //InetAddress.getByName("192.168.1.1")
        getWin.dispose();
        window.setIP(getWin.IP);
        window.setPort(getWin.Port);
        getWin.dispatchEvent(new WindowEvent(getWin, WindowEvent.WINDOW_CLOSING));
        //window.ipField.setText("Pi: "+server.IP.toString()+" Port: "+server.port+" | ");
        //window.connectionBar.setValues100);
        window.setup();
        window.setVisible(true);
    }
}
