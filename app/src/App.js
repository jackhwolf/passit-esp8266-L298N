import React from 'react';
import './App.css';
import Container from 'react-bootstrap/Container'
import Row from 'react-bootstrap/Row'
import Col from 'react-bootstrap/Col'
import Button from 'react-bootstrap/Button'

class App extends React.Component {

  constructor(props) {
    super(props);
    this.state = {
      url: "http://10.0.0.37",
      mostRecentMessage: ""
    }
    this.sendAndUpdate = this.sendAndUpdate.bind(this)
  }

  async sendRequestToBoard(endp) {
    let resp = await fetch(this.state.url + endp).then(resp => resp.text())
    return resp
  }

  async sendAndUpdate(endp) {
    let resp = await this.sendRequestToBoard(endp)
    this.setState({ mostRecentMessage: resp })
    console.log(resp)   
  }

  render() {
    return (
      <div className="App">
        <Container>
          <Row>
            <Col><Button onClick={() => this.sendAndUpdate("/motor?power=1")}>Motor on</Button></Col>
            <Col><Button onClick={() => this.sendAndUpdate("/motor?power=0")}>Motor off</Button></Col>
            <Col><Button onClick={() => this.sendAndUpdate("/motor?direction=2")}>Motor switch direction</Button></Col>
          </Row>
        </Container>
      </div>
    );
  }
}

export default App;
