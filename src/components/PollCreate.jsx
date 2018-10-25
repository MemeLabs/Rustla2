import React from 'react';
import PropTypes from 'prop-types';
import { compose } from 'redux';
import { connect } from 'react-redux';

import Checkbox from './Checkbox';
import { createPoll } from '../actions';
import MainLayout from './MainLayout';

import '../css/Polls';

const OptionInput = ({ value, index, onChange }) => (
  <div className='form-group'>
    <div className='col-sm-12'>
      <input
        className='form-control'
        type='text'
        placeholder={'Enter poll option'}
        value={value}
        onChange={event => onChange(event, index)}
      />
    </div>
  </div>
);

OptionInput.propTypes = {
  value: PropTypes.string.isRequired,
  index: PropTypes.number.isRequired,
  onChange: PropTypes.func.isRequired,
};

class PollCreateForm extends React.Component {
  constructor(props) {
    super(props);

    this.state = {
      options: new Array(this.props.minOptionCount).fill(''),
    };
  }

  handleSubmit = (event) => {
    event.preventDefault();

    const payload = {
      subject: event.target.elements.subject.value,
      options: this.state.options.filter(option => option !== ''),
      multi_vote: event.target.elements.multi_vote.checked,
    };

    if (!payload.options.length) {
      return;
    }

    this.props.onSubmit(payload);
  }

  handleOptionChange = (event, index) => {
    const options = this.state.options.slice();

    options[index] = event.target.value;

    let {length} = options;
    while (length > this.props.minOptionCount && options[-- length] === '') {
      options.pop();
    }

    this.setState({options});
  }

  render() {
    const options = [
      ...this.state.options,
      '',
    ];

    const optionInputs = options.map((value, i) => (
      <OptionInput
        key={i}
        index={i}
        value={value}
        onChange={this.handleOptionChange}
      />
    ));

    return (
      <form
        className='form-horizontal'
        onSubmit={this.handleSubmit}
      >
        <div className='form-group'>
          <div className='col-sm-12'>
            <input
              className='form-control'
              type='text'
              name='subject'
              placeholder={'Enter poll title'}
            />
          </div>
        </div>
        {optionInputs}
        <div className='form-group'>
          <div className='col-sm-12'>
            <label>
              <Checkbox
                className='form-control'
                id='poll-multi-vote'
                name='multi_vote'
                defaultChecked={this.state.multi_vote}
              />
              <span className="poll-multi-vote-label">Allow multiple poll answers</span>
            </label>
          </div>
        </div>
        <div className='form-group'>
          <div className='col-sm-12'>
            <button
              type='submit'
              className='btn btn-primary'
              disabled={false}
            >
              Save Poll
            </button>
          </div>
        </div>
      </form>
    );
  }
}

PollCreateForm.defaultProps = {
  minOptionCount: 3,
}

PollCreateForm.propTypes = {
  minOptionCount: PropTypes.number,
  onSubmit: PropTypes.func.isRequired,
}

const PollCreate = ({ history, createPoll }) => (
  <MainLayout history={history}>
    <div className='container'>
      <h1 className='text-center'>Create Poll</h1>
      <PollCreateForm onSubmit={createPoll} />
    </div>
  </MainLayout>
);

PollCreate.propTypes = {
  history: PropTypes.object.isRequired,
  createPoll: PropTypes.func.isRequired,
};

function mapDispatchToProps(dispatch, props) {
  return {
    createPoll(poll) {
      return dispatch(createPoll(poll, props.history));
    },
  };
}

export default compose(
  connect(() => ({}), mapDispatchToProps),
)(PollCreate);

