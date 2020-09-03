import subprocess
import os
import config

'''
This is a helper class to execute system
commands locally and remotely
'''
class CmdHelper:
    def __init__(self):
        self.sshKeyPath = config.SSH_KEY_PATH
        self.username = config.USER_NAME
    
    def executeCmdBlocking(self, cmd, redirectOutputPath=None):
        try:
            if redirectOutputPath:
                f = open(redirectOutputPath, "w")
                p = subprocess.Popen(cmd, shell=True, stdout=f, stderr=f)
                f.close()
            else:
                p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
            output, err = p.communicate()
            return [output, err]
        except OSError as e:
            return [None, e]
        except ValueError as e:
            return [None, e]
        except Exception as e:
            return [None, e]

    def executeCmdNonBlocking(self, cmd, redirectOutputPath=None):
        try:
            cmd = "sudo " + cmd
            if redirectOutputPath:
                f = open(redirectOutputPath, "w")
                p = subprocess.Popen(cmd, shell=True, stdout=f, stderr=f)
            else:
                p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
            return [p, None]
        except OSError as e:
            return [None, e]
        except ValueError as e:
            return [None, e]
        except Exception as e:
            return [None, e]

    def uploadToServer(self, localPath, remoteAddress, remotePath, blocking=True):
        cmd = "sudo scp -o StrictHostKeyChecking=no -r "        
        usernameStr = ""
        if self.sshKeyPath:
            cmd = cmd + " -i " + self.sshKeyPath
        if self.username:
            usernameStr = " " + self.username + "@"
        
        cmd = cmd + " " + localPath + " "
        cmd = cmd + usernameStr
        cmd = cmd + remoteAddress
        cmd = cmd + ":" + remotePath
        if blocking:
            return self.executeCmdBlocking(cmd)
        else:
            return self.executeCmdNonBlocking(cmd)

    def downloadFromServer(self, localPath, remoteAddress, remotePath):
        cmd = "sudo scp -o StrictHostKeyChecking=no -r "
        if self.sshKeyPath:
            cmd = cmd + " -i " + self.sshKeyPath 
        if self.username:
            cmd = cmd + " " + self.username +"@" 

        cmd = cmd + remoteAddress
        cmd = cmd + ":" + remotePath
        cmd = cmd + " " + localPath
        return self.executeCmdBlocking(cmd)
    
    def executeCmdRemotely(self, cmd, remoteAddress, blocking=True, redirectOutputPath=None):
        cmd = "\"sudo " + cmd + "\""
        fullCommand = "ssh -o StrictHostKeyChecking=no "
        if self.sshKeyPath:
            fullCommand = fullCommand + " -i " + self.sshKeyPath 
        if self.username:
            fullCommand = fullCommand + " " + self.username +"@" 
        
        fullCommand = fullCommand + remoteAddress
        fullCommand = fullCommand + " " + cmd
        if blocking:
            return self.executeCmdBlocking(fullCommand, redirectOutputPath)
        else:
            return self.executeCmdNonBlocking(fullCommand, redirectOutputPath)
        
            
