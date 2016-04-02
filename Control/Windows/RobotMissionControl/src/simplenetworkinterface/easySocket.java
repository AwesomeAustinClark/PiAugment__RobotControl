/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package simplenetworkinterface;

import java.io.BufferedReader;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.util.Arrays;

/**
 *
 * @author Grady
 */
public class easySocket {
    InetAddress IP;
    int port = 4000;
    InetAddress remoteIP;
    int remotePort = 4000;
    DatagramSocket socket;
    BufferedReader in;
    String allReadData[] = new String[2000];
    byte[] receiveData = new byte[10];
    byte[] sendData = new byte[10];
    DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
    DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length/*, IPAddress, 9876*/);

    void send(byte[] msg_) throws IOException{
        if(msg_.length==0){
            return;
        }
        socket.send(new DatagramPacket(msg_, msg_.length,remoteIP, remotePort));
    }
    
    void send(String msg_) throws IOException{
        send(msg_.getBytes());
    }
    
    String read(int time_ms) throws IOException{
        String str;
        socket.setSoTimeout(time_ms);
        try{
            socket.receive(receivePacket);
            str = new String(receiveData).trim();
        }catch(java.net.SocketTimeoutException e){
            str = new String();
        }
        socket.setSoTimeout(0);
        return str;
    }
    
    void close(){
        if(!socket.isClosed()){
            socket.close();
        }
    }
    
    void setup(int myPort_, InetAddress remoteIP_, int remotePort_) throws IOException {
        port = myPort_;
        IP = InetAddress.getLocalHost();
        remotePort = remotePort_;
        socket = new DatagramSocket(port);
        remoteIP = remoteIP_;
        //remoteIP =  receivePacket.getAddress();
        //remotePort = receivePacket.getPort();
    }  
    
}
